#include <vvv.h>

#if false && defined(_WIN32)
#include <wil/com.h>
#include <d3d12.h>
#include <dxgi1_6.h>

namespace vvv
{
	namespace com
	{
		template<typename T_FUNC, typename T_RET = HRESULT>
		struct dll_proc
		{
			const char* const m_symbol{nullptr};
			T_FUNC			  m_function{nullptr};

			dll_proc(const char* symbol) : m_symbol{symbol} { }
			dll_proc(dll_proc&&) = default;
			~dll_proc()			 = default;

			T_RET operator()()
			{
				return m_function();
			}

			template<typename... T_ARGS>
			T_RET operator()(T_ARGS&&... args)
			{
				return m_function(std::forward<T_ARGS>(args)...);
			}
		};

		struct dll_base
		{
			HMODULE m_handle{nullptr};

		protected:
			template<typename T>
			result<void> get_proc_impl(const char* symbol_name, T* target_func)
			{
				vvv_EXPECT_VALID(symbol_name);
				vvv_EXPECT_VALID(target_func);
				vvv_EXPECT_VALID(m_handle);

				*target_func = (T)::GetProcAddress(m_handle, symbol_name);
				if (!*target_func)
				{
					return vvv_NEW_ERROR(runtime_error::not_specified{});
				}

				return {};
			}

			template<typename T_PROC>
			auto get_proc(T_PROC& proc)
			{
				return get_proc_impl(proc.m_symbol, &proc.m_function);
			}

			template<typename T, typename... T_ARGS>
			static inline result<std::shared_ptr<T>> factory(const char* name, T_ARGS&&... args)
			{
				vvv_EXPECT_VALID(name);

				auto handle = ::LoadLibraryA(name);
				if (!handle)
				{
					return vvv_NEW_ERROR(runtime_error::not_specified{});
				}

				return std::make_unique<T>(handle, std::forward<T_ARGS>(args)...);
			}

		public:
			dll_base(HMODULE h) : m_handle(h) { }

			dll_base(dll_base&&) = default;

			virtual ~dll_base()
			{
				if (m_handle)
				{
					::FreeLibrary(m_handle);
				}
			}
		};

		struct dxgi_iface : public dll_base
		{
			typedef HRESULT(WINAPI* PFN_CreateDXGIFactory2)(UINT Flags, REFIID riid, _COM_Outptr_ void** ppFactory);
			typedef HRESULT(WINAPI* PFN_DXGIGetDebugInterface1)(UINT Flags, REFIID riid, _COM_Outptr_ void** pDebug);

			dll_proc<PFN_CreateDXGIFactory2>	 m_CreateFactory{"CreateDXGIFactory2"};
			dll_proc<PFN_DXGIGetDebugInterface1> m_GetDebugInterface{"DXGIGetDebugInterface1"};

		protected:
			result<void> init_procs()
			{
				vvv_CHECK(get_proc(m_CreateFactory));
				vvv_CHECK(get_proc(m_GetDebugInterface));

				return {};
			}

		public:
			dxgi_iface(HMODULE handle) : dll_base(handle) { }

			virtual ~dxgi_iface() = default;

			static inline result<std::shared_ptr<dxgi_iface>> factory(const char* name = "dxgi.dll")
			{
				vvv_EXPECT_VALID(name);
				vvv_AUTO(self, dll_base::factory<dxgi_iface>(name));
				vvv_CHECK(self->init_procs());
				return self;
			}
		};

		template<typename T>
		struct com_obj
		{
			wil::com_ptr_nothrow<T> m_obj;

			com_obj(wil::com_ptr_nothrow<T> obj) : m_obj{obj} { }

			virtual ~com_obj() = default;

			wil::com_ptr_nothrow<T>& get()
			{
				return m_obj;
			}

			T* operator->()
			{
				return m_obj.get();
			}

			const T* operator->() const
			{
				return m_obj.get();
			}
		};

		template<typename T>
		struct nameable_com_obj : com_obj<T>
		{
			using com_obj<T>::com_obj;
			using com_obj<T>::m_obj;

			virtual ~nameable_com_obj() = default;

			inline result<void> set_name(const char* name)
			{
				wil::com_ptr_nothrow<ID3D12Object> obj;
				if (auto hr = m_obj->QueryInterface(IID_ID3D12Object, obj.put_void()); SUCCEEDED(hr))
				{
					std::mbstate_t state = std::mbstate_t();
					std::size_t	   len	 = 1 + std::mbsrtowcs(nullptr, &name, 0, &state);

					std::array<wchar_t, 128> stack_tmp;
					if (len < stack_tmp.size())
					{
						std::mbsrtowcs(stack_tmp.data(), &name, len, &state);
						obj->SetName(stack_tmp.data());
					}
					else
					{
						std::vector<wchar_t> heap_tmp(len);
						std::mbsrtowcs(heap_tmp.data(), &name, len, &state);
						obj->SetName(heap_tmp.data());
					}

					return {};
				}

				return vvv_NEW_ERROR(runtime_error::not_specified{});
			}
		};

		template<typename T_FACTORY>
		struct com_factory_helper
		{
			using factory = typename T_FACTORY;

			static inline auto query(wil::com_ptr_nothrow<typename factory::create_type>& ct) -> result<typename factory::com_ptr>
			{
				typename factory::com_ptr finalType;
				if (auto hr = ct->QueryInterface(factory::iid, finalType.put_void()); SUCCEEDED(hr))
				{
					return finalType;
				}

				return vvv_NEW_ERROR(runtime_error::not_specified{});
			}
		};

		struct dxgi_factory
		{
			using create_type					= IDXGIFactory2;
			static inline const auto create_iid = IID_IDXGIFactory2;

			using type					 = IDXGIFactory7;
			static inline const auto iid = IID_IDXGIFactory7;

			using base_type = com_obj<type>;

			using com_ptr = wil::com_ptr_nothrow<type>;

			static inline result<com_ptr> com_factory(UINT flags)
			{
				wil::com_ptr_nothrow<create_type> factory2;

				if (auto hr = CreateDXGIFactory2(flags, create_iid, factory2.put_void()); SUCCEEDED(hr))
				{
					return com_factory_helper<dxgi_factory>::query(factory2);
				}
				else
				{
					return vvv_err_not_specified(fmt::format("CreateDXGIFactory2 failed: {}", hr));
				}
			}

			template<typename T, typename T_BASE_COM = com_obj<T>>
			struct ref_base : public T_BASE_COM
			{
				std::shared_ptr<base_type> m_dxgi_factory;

				template<typename T_ARG>
				ref_base(std::shared_ptr<base_type> self_ref, T_ARG&& obj) : T_BASE_COM(std::forward<T_ARG>(obj))
																		   , m_dxgi_factory(self_ref)
				{ }

				virtual ~ref_base() = default;
			};

			template<typename T>
			using nameable_ref_base = ref_base<T, nameable_com_obj<T>>;
		};

		struct adapter
		{
			using create_type					= IDXGIAdapter1;
			static inline const auto create_iid = IID_IDXGIAdapter1;

			using type					 = IDXGIAdapter4;
			static inline const auto iid = IID_IDXGIAdapter4;

			using adapter_index = unsigned int;
			using base_type		= dxgi_factory::ref_base<type>;

			using com_ptr = wil::com_ptr_nothrow<type>;

			static inline result<com_ptr> com_factory_by_preference(adapter_index idx, DXGI_GPU_PREFERENCE pref, dxgi_factory::com_ptr factory_ref)
			{
				wil::com_ptr_nothrow<create_type> adapter1;

				if (auto hr = factory_ref->EnumAdapterByGpuPreference(idx, pref, create_iid, adapter1.put_void()); SUCCEEDED(hr))
				{
					com_ptr finalType;
					if (auto hr = adapter1->QueryInterface(iid, finalType.put_void()); SUCCEEDED(hr))
					{
						return finalType;
					}
					else
					{
						return vvv_err_not_specified(fmt::format("adapter1->QueryInterface failed: {}", hr));
					}
				}
				else
				{
					return vvv_err_not_specified(fmt::format("EnumAdapterByGpuPreference failed: {}", hr));
				}
			}

			template<typename T_SELECTOR>
			static inline result<com_ptr> com_factory_enum(T_SELECTOR selector, dxgi_factory::com_ptr factory_ref)
			{
				wil::com_ptr_nothrow<create_type> adapter1;

				for (int idx = 0; [&]()
					 {
						 auto hr = (*factory_ref)->EnumAdapters1(idx, adapter1.put());
						 return SUCCEEDED(hr);
					 }();
					 ++idx)
				{
					com_ptr finalType;
					if (auto hr = adapter1->QueryInterface(iid, finalType.put_void()); SUCCEEDED(hr))
					{
						if (selector(finalType))
						{
							return finalType;
						}
					}
					else
					{
						return vvv_err_not_specified(fmt::format("adapter1->QueryInterface failed: {}", hr));
					}
				}

				return vvv_err_not_specified(fmt::format("EnumAdapters1 did not lead to the discovery of any valid adapters"));
			}

			template<typename T, typename T_BASE_COM = com_obj<T>>
			struct ref_base : public T_BASE_COM
			{
				std::shared_ptr<base_type> m_adapter;

				template<typename T_ARG>
				ref_base(std::shared_ptr<base_type> adapter_ref, T_ARG&& obj) : T_BASE_COM(std::forward<T_ARG>(obj))
																			  , m_adapter(adapter_ref)
				{ }

				virtual ~ref_base() = default;
			};

			template<typename T>
			using nameable_ref_base = ref_base<T, nameable_com_obj<T>>;
		};

		struct output
		{
			using create_type					= IDXGIOutput;
			static inline const auto create_iid = IID_IDXGIOutput;

			using type					 = IDXGIOutput6;
			static inline const auto iid = IID_IDXGIOutput6;

			using base_type = adapter::ref_base<type>;
			using com_ptr	= wil::com_ptr_nothrow<type>;

			using output_index = unsigned int;

			static inline result<com_ptr> com_factory(output_index idx, adapter::com_ptr adapter_ref)
			{
				wil::com_ptr_nothrow<create_type> op;
				if (auto hr = adapter_ref->EnumOutputs(idx, op.put()); SUCCEEDED(hr))
				{
					return com_factory_helper<output>::query(op);
				}
				else
				{
					return vvv_err_not_specified(fmt::format("adapter_ref->EnumOutputs failed: {}", hr));
				}
			}

			// Leaf type, no ref
		};
	} // namespace com

	using adapter_index = com::adapter::adapter_index;

	struct adapter
		: public com::adapter::base_type
		, public std::enable_shared_from_this<adapter>
	{
		using com::adapter::base_type::base_type;

		virtual ~adapter() = default;

		result<DXGI_ADAPTER_DESC3> get_desc3()
		{
			DXGI_ADAPTER_DESC3 desc;
			if (auto hr = m_obj->GetDesc3(&desc); SUCCEEDED(hr))
			{
				return desc;
			}
			throw std::runtime_error("Adapter GetDesc3 failed");
		}

		static inline result<std::unique_ptr<adapter>> factory(adapter_index idx, DXGI_GPU_PREFERENCE pref, std::shared_ptr<com::dxgi_factory::base_type> factory_ref)
		{
			vvv_AUTO(res, com::adapter::com_factory_by_preference(idx, pref, factory_ref->m_obj));
			return std::make_unique<adapter>(factory_ref, std::move(res));
		}

		template<typename T_SELECTOR>
		static inline result<std::unique_ptr<adapter>> factory(T_SELECTOR selector, std::shared_ptr<com::dxgi_factory::base_type> factory_ref)
		{
			vvv_AUTO(res, com::adapter::com_factory_enum(selector, factory_ref->m_obj));
			return std::make_unique<adapter>(factory_ref, std::move(res));
		}
	};

	struct dxgi_factory
		: public com::dxgi_factory::base_type
		, public std::enable_shared_from_this<dxgi_factory>
	{
		using com::dxgi_factory::base_type::base_type;

		virtual ~dxgi_factory() = default;

		result<std::shared_ptr<adapter>> create_adapter(adapter_index idx, DXGI_GPU_PREFERENCE pref)
		{
			return adapter::factory(idx, pref, shared_from_this());
		}

		template<typename T_SELECTOR>
		result<std::shared_ptr<adapter>> create_adapter(T_SELECTOR selector)
		{
			return adapter::factory(selector, shared_from_this());
		}

		static inline result<std::shared_ptr<dxgi_factory>> factory(UINT flags)
		{
			vvv_AUTO(res, com::dxgi_factory::com_factory(flags));
			return std::make_shared<dxgi_factory>(std::move(res));
		}
	};

	struct display_helper
	{
		std::shared_ptr<dxgi_factory> m_dxgi_factory;

		display_helper()
		{
			leaf::try_catch(
				[&]()
				{
					UINT dxgiFactoryFlags = 0;
					vvv_ASSIGN(m_dxgi_factory, dxgi_factory::factory(dxgiFactoryFlags));
				});
		}

		result<std::shared_ptr<adapter>> find_hardware_adapter_for_hwnd(HWND hwnd)
		{
			while (auto a = m_dxgi_factory->create_adapter(
					   [&](com::adapter::com_ptr& a)
					   {
						   DXGI_ADAPTER_DESC3 desc;
						   if (auto hr = a->GetDesc3(&desc); SUCCEEDED(hr))
						   {
							   // desc.AdapterLuid

						   }
						   return false;
					   }))
			{
			}

			return vvv_err_not_specified("unable to find suitable adapter for hwnd");
		}

		result<std::shared_ptr<adapter>> choose_hardware_adapter()
		{
			UINT adapterIndex = 0;

			while (auto adapter = m_dxgi_factory->create_adapter(0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE))
			{
				++adapterIndex;

				if (auto desc = (*adapter)->get_desc3())
				{
					if ((*desc).Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
					{
						continue;
					}

					return std::move(*adapter);
				}
			}

			return vvv_err_not_specified("unable to find suitable adapter");
		}

		void associate_hwnd(HWND hwnd, UINT flags = DXGI_MWA_NO_ALT_ENTER | /*DXGI_MWA_NO_WINDOW_CHANGES |*/ DXGI_MWA_NO_PRINT_SCREEN)
		{
			m_dxgi_factory->m_obj->MakeWindowAssociation(hwnd, flags);
		}
	};

	/*
	 */

} // namespace vvv
#endif // #ifdef _WIN32