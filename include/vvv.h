#pragma once

#include "details/vvv_error_handling.h"

#include <volk.h>

#include <algorithm>
#include <array>
#include <filesystem>

namespace vvv
{
	inline void init(int argc, char** argv)
	{
		::_wputenv_s(L"VK_LAYER_PATH", std::filesystem::path(argv[0]).parent_path().c_str());
	}

	template<typename T_VAL>
	inline constexpr auto enum_value(T_VAL val) -> std::underlying_type<T_VAL>::type
	{
		return static_cast<std::underlying_type<T_VAL>::type>(val);
	}

	inline constexpr const char* VkResult_string(VkResult res)
	{
#define VkResult_ENUMSTR(VAL)                                                                                                                                                      \
	case VAL:                                                                                                                                                                      \
		return #VAL

		switch (res)
		{
			VkResult_ENUMSTR(VK_SUCCESS);
			VkResult_ENUMSTR(VK_NOT_READY);
			VkResult_ENUMSTR(VK_TIMEOUT);
			VkResult_ENUMSTR(VK_EVENT_SET);
			VkResult_ENUMSTR(VK_EVENT_RESET);
			VkResult_ENUMSTR(VK_INCOMPLETE);
			VkResult_ENUMSTR(VK_ERROR_OUT_OF_HOST_MEMORY);
			VkResult_ENUMSTR(VK_ERROR_OUT_OF_DEVICE_MEMORY);
			VkResult_ENUMSTR(VK_ERROR_INITIALIZATION_FAILED);
			VkResult_ENUMSTR(VK_ERROR_DEVICE_LOST);
			VkResult_ENUMSTR(VK_ERROR_MEMORY_MAP_FAILED);
			VkResult_ENUMSTR(VK_ERROR_LAYER_NOT_PRESENT);
			VkResult_ENUMSTR(VK_ERROR_EXTENSION_NOT_PRESENT);
			VkResult_ENUMSTR(VK_ERROR_FEATURE_NOT_PRESENT);
			VkResult_ENUMSTR(VK_ERROR_INCOMPATIBLE_DRIVER);
			VkResult_ENUMSTR(VK_ERROR_TOO_MANY_OBJECTS);
			VkResult_ENUMSTR(VK_ERROR_FORMAT_NOT_SUPPORTED);
			VkResult_ENUMSTR(VK_ERROR_FRAGMENTED_POOL);
			VkResult_ENUMSTR(VK_ERROR_UNKNOWN);
			VkResult_ENUMSTR(VK_ERROR_OUT_OF_POOL_MEMORY);
			VkResult_ENUMSTR(VK_ERROR_INVALID_EXTERNAL_HANDLE);
			VkResult_ENUMSTR(VK_ERROR_FRAGMENTATION);
			VkResult_ENUMSTR(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
			VkResult_ENUMSTR(VK_PIPELINE_COMPILE_REQUIRED);
			VkResult_ENUMSTR(VK_ERROR_SURFACE_LOST_KHR);
			VkResult_ENUMSTR(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR);
			VkResult_ENUMSTR(VK_SUBOPTIMAL_KHR);
			VkResult_ENUMSTR(VK_ERROR_OUT_OF_DATE_KHR);
			VkResult_ENUMSTR(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR);
			VkResult_ENUMSTR(VK_ERROR_VALIDATION_FAILED_EXT);
			VkResult_ENUMSTR(VK_ERROR_INVALID_SHADER_NV);
			VkResult_ENUMSTR(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
			VkResult_ENUMSTR(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
			VkResult_ENUMSTR(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
			VkResult_ENUMSTR(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
			VkResult_ENUMSTR(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
			VkResult_ENUMSTR(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
			VkResult_ENUMSTR(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
			VkResult_ENUMSTR(VK_ERROR_NOT_PERMITTED_KHR);
			VkResult_ENUMSTR(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
			VkResult_ENUMSTR(VK_THREAD_IDLE_KHR);
			VkResult_ENUMSTR(VK_THREAD_DONE_KHR);
			VkResult_ENUMSTR(VK_OPERATION_DEFERRED_KHR);
			VkResult_ENUMSTR(VK_OPERATION_NOT_DEFERRED_KHR);
#ifdef VK_ENABLE_BETA_EXTENSIONS
			VkResult_ENUMSTR(VK_ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR);
#endif
			VkResult_ENUMSTR(VK_ERROR_COMPRESSION_EXHAUSTED_EXT);
			VkResult_ENUMSTR(VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT);
		default:
			return "<unknown>";
		};
	};

	static inline result<void> load_vulkan()
	{
		if (auto r = volkInitialize(); r != VK_SUCCESS)
		{
			return vvv_err_not_specified("volkInitialize failed!");
		}

		return {};
	}

	struct physical_device_info
	{
		VkPhysicalDevice					m_physical_device;
		VkPhysicalDeviceProperties			m_device_properties;
		VkPhysicalDeviceFeatures			m_device_features;
		VkPhysicalDeviceIDProperties		m_device_id_properties;
		VkPhysicalDeviceMemoryProperties	m_device_memory_properties;
		std::vector<VkDisplayPropertiesKHR> m_ext_display_properties; // Displays that are NOT connected to the desktop

		physical_device_info(VkPhysicalDevice v) : m_physical_device(v), m_device_id_properties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES}
		{
			vkGetPhysicalDeviceProperties(m_physical_device, &m_device_properties);
			vkGetPhysicalDeviceFeatures(m_physical_device, &m_device_features);

			VkPhysicalDeviceProperties2 physDeviceProps = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
			physDeviceProps.pNext						= &m_device_id_properties;
			vkGetPhysicalDeviceProperties2(m_physical_device, &physDeviceProps);

			vkGetPhysicalDeviceMemoryProperties(m_physical_device, &m_device_memory_properties);

			uint32_t physical_device_display_properties_count = 0;
			vkGetPhysicalDeviceDisplayPropertiesKHR(m_physical_device, &physical_device_display_properties_count, nullptr);
			if (physical_device_display_properties_count > 0)
			{
				m_ext_display_properties.resize(physical_device_display_properties_count);
				vkGetPhysicalDeviceDisplayPropertiesKHR(m_physical_device, &physical_device_display_properties_count, m_ext_display_properties.data());
			}
		}

		std::tuple<uint32_t, uint32_t, uint32_t> get_version() const
		{
			return {VK_VERSION_MAJOR(m_device_properties.apiVersion), VK_VERSION_MINOR(m_device_properties.apiVersion), VK_VERSION_PATCH(m_device_properties.apiVersion)};
		}
	};

	// TODO: queue family selection is a clusterfuck and probably better handled with a lookup table based on the device UUID
	// since selecting exclusive queues isn't necessarily the best.

	struct queue_family_desc
	{
		enum class type : uint32_t
		{
			GFX = 0,
			TRANSFER,
			COMPUTE,
			VIDEO_DECODE,
			VIDEO_ENCODE,
			COUNT
		};

		size_t					m_family_index;
		VkQueueFamilyProperties m_properties{};
		type					m_type{type::COUNT};

		bool m_gfx{false};
		bool m_compute{false};
		bool m_transfer{false};
		bool m_sparse_binding{false};
		bool m_protected{false};
		bool m_video_decode{false};
		bool m_video_encode{false};

		bool m_supports_present{false};

		queue_family_desc() = default;

		queue_family_desc(size_t idx, const VkQueueFamilyProperties& props, type t, VkPhysicalDevice phys_dev) : m_family_index{idx}, m_properties{props}, m_type{t}
		{
			m_gfx			 = (props.queueFlags & VK_QUEUE_GRAPHICS_BIT);
			m_compute		 = (props.queueFlags & VK_QUEUE_COMPUTE_BIT);
			m_transfer		 = (props.queueFlags & VK_QUEUE_TRANSFER_BIT);
			m_sparse_binding = (props.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT);
			m_protected		 = (props.queueFlags & VK_QUEUE_PROTECTED_BIT);
			m_video_decode	 = (props.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR);
			m_video_encode	 = (props.queueFlags & VK_QUEUE_VIDEO_ENCODE_BIT_KHR);

#if defined(VK_KHR_win32_surface)
			m_supports_present = vkGetPhysicalDeviceWin32PresentationSupportKHR(phys_dev, m_family_index);
#endif
		}

		operator bool() const
		{
			return m_type != type::COUNT;
		}
	};

	struct physical_device
	{
		std::unique_ptr<physical_device_info> m_device_info;
		physical_device(std::unique_ptr<physical_device_info>&& di) : m_device_info{std::move(di)} { }

		result<std::vector<VkQueueFamilyProperties>> get_queue_family_properties()
		{
			uint32_t count = 0;
			if (vkGetPhysicalDeviceQueueFamilyProperties(m_device_info->m_physical_device, &count, nullptr); count > 0)
			{
				std::vector<VkQueueFamilyProperties> queue_family_props(count);

				vkGetPhysicalDeviceQueueFamilyProperties(m_device_info->m_physical_device, &count, queue_family_props.data());
				return queue_family_props;
			}
			else
			{
				return vvv_err_not_specified("Unable to discover any valid devices");
			}
		}

		static inline bool get_queue_family_support(const VkQueueFamilyProperties& props, VkQueueFlagBits flag_bits)
		{
			return ((props.queueFlags & flag_bits) == flag_bits);
		}

		struct exclusive_queue_families
		{
			size_t gfx_family{0xffffffffull};
			size_t transfer_family{0xffffffffull};
			size_t compute_family{0xffffffffull};
			size_t video_decode_family{0xffffffffull};
			size_t video_encode_family{0xffffffffull};
		};

		result<exclusive_queue_families> find_exclusive_queue_families(const std::vector<VkQueueFamilyProperties>& props)
		{
			exclusive_queue_families res;

			auto find_exclusive_family = [&](auto type_bits)
			{
				for (auto i = 0ull; i < props.size(); ++i)
				{
					if (i != res.gfx_family && get_queue_family_support(props[i], type_bits))
					{
						return i;
					}
				}
				return 0xffffffffull;
			};

			res.gfx_family			= find_exclusive_family(VK_QUEUE_GRAPHICS_BIT);
			res.transfer_family		= find_exclusive_family(VK_QUEUE_TRANSFER_BIT);
			res.compute_family		= find_exclusive_family(VK_QUEUE_COMPUTE_BIT);
			res.video_decode_family = find_exclusive_family(VK_QUEUE_VIDEO_DECODE_BIT_KHR);
			res.video_encode_family = find_exclusive_family(VK_QUEUE_VIDEO_ENCODE_BIT_KHR);

			if (res.gfx_family >= props.size())
			{
				return vvv_err_not_specified("Unable to discover any valid gfx_family");
			}

			return res;
		}

		result<exclusive_queue_families> get_exclusive_queue_families()
		{
			vvv_LEAF_AUTO(queue_family_props, get_queue_family_properties());
			return find_exclusive_queue_families(queue_family_props);
		}

		using queue_families = std::array<queue_family_desc, enum_value(queue_family_desc::type::COUNT)>;

		result<queue_families> get_queue_families()
		{
			queue_families res;
			vvv_LEAF_AUTO(queue_family_props, get_queue_family_properties());
			vvv_LEAF_AUTO(exclusive_families, find_exclusive_queue_families(queue_family_props));

			auto assign_queue_family = [&](auto var, auto flag)
			{
				if (var <= queue_family_props.size())
				{
					res[enum_value(flag)] = queue_family_desc(var, queue_family_props[var], flag, m_device_info->m_physical_device);
				}
			};

			assign_queue_family(exclusive_families.gfx_family, queue_family_desc::type::GFX);
			assign_queue_family(exclusive_families.transfer_family, queue_family_desc::type::TRANSFER);
			assign_queue_family(exclusive_families.compute_family, queue_family_desc::type::COMPUTE);
			assign_queue_family(exclusive_families.video_decode_family, queue_family_desc::type::VIDEO_DECODE);
			assign_queue_family(exclusive_families.video_encode_family, queue_family_desc::type::VIDEO_ENCODE);

			return res;
		}
	};

	struct surface
	{
		VkSurfaceKHR m_surface;
		surface(VkSurfaceKHR surf) : m_surface(surf) { }
	};

	struct instance
	{
		struct raw_VkInstance
		{
			raw_VkInstance(VkInstance inst) : m_value{inst} { }

			~raw_VkInstance()
			{
				if (m_value)
				{
					vkDestroyInstance(m_value, nullptr);
				}
			}

			VkInstance m_value{nullptr};

			operator VkInstance()
			{
				return m_value;
			}

			operator VkInstance() const
			{
				return m_value;
			}
		};
		std::unique_ptr<raw_VkInstance> m_vulkan_instance;

		struct raw_VkDebugReportCallbackEXT
		{
			raw_VkDebugReportCallbackEXT(VkInstance inst, VkDebugReportCallbackEXT cb) : m_vulkan_instance{inst}, m_value{cb} { }

			~raw_VkDebugReportCallbackEXT()
			{
				if (m_value && m_vulkan_instance)
				{
					vkDestroyDebugReportCallbackEXT(m_vulkan_instance, m_value, nullptr);
				}
			}

			VkInstance				 m_vulkan_instance{nullptr};
			VkDebugReportCallbackEXT m_value{nullptr};

			operator VkDebugReportCallbackEXT()
			{
				return m_value;
			}

			operator VkDebugReportCallbackEXT() const
			{
				return m_value;
			}
		};
		std::unique_ptr<raw_VkDebugReportCallbackEXT> m_instance_debug_callback;

		instance()
		{
			VkApplicationInfo application_info =
				{VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "Loopunit", VK_MAKE_VERSION(0, 0, 1), "vvv", VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_3};

			VkInstanceCreateInfo instance_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &application_info, 0, nullptr, 0, nullptr};

			// Extensions

			uint32_t availiable_extension_count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &availiable_extension_count, nullptr);
			std::vector<VkExtensionProperties> available_extensions(availiable_extension_count);
			vkEnumerateInstanceExtensionProperties(nullptr, &availiable_extension_count, available_extensions.data());

			std::vector<const char*> enabled_extensions;
			auto					 find_and_add_extension = [&](const char* ext_name)
			{
				for (auto& ext : available_extensions)
				{
					if (::stricmp(ext.extensionName, ext_name) == 0)
					{
						enabled_extensions.push_back(ext_name);
						return;
					}
				}
			};

			find_and_add_extension(VK_KHR_SURFACE_EXTENSION_NAME);
			find_and_add_extension(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME);
			find_and_add_extension(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
			find_and_add_extension(VK_KHR_DISPLAY_EXTENSION_NAME);

#if defined(_WIN32)
			find_and_add_extension(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
			find_and_add_extension(VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME);
			find_and_add_extension(VK_KHR_WIN32_KEYED_MUTEX_EXTENSION_NAME);
			find_and_add_extension(VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME);
			find_and_add_extension(VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME);
			// find_and_add_extension(VK_NV_ACQUIRE_WINRT_DISPLAY_EXTENSION_NAME);
#else
			find_and_add_extension(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
			if (enabled_extensions.size() > 0)
			{
				instance_create_info.enabledExtensionCount	 = static_cast<uint32_t>(enabled_extensions.size());
				instance_create_info.ppEnabledExtensionNames = enabled_extensions.data();
			}

			// Layers

			uint32_t availiable_layer_count = 0;
			vkEnumerateInstanceLayerProperties(&availiable_layer_count, nullptr);
			std::vector<VkLayerProperties> available_layers(availiable_layer_count);
			vkEnumerateInstanceLayerProperties(&availiable_layer_count, available_layers.data());

			std::vector<const char*> enabled_layers;
			auto					 find_and_add_layer = [&](const char* layer_name)
			{
				for (auto& l : available_layers)
				{
					if (::stricmp(l.layerName, layer_name) == 0)
					{
						enabled_layers.push_back(layer_name);
						return;
					}
				}
			};

			find_and_add_layer("VK_LAYER_KHRONOS_validation");

			if (enabled_layers.size() > 0)
			{
				instance_create_info.enabledLayerCount	 = static_cast<uint32_t>(enabled_layers.size());
				instance_create_info.ppEnabledLayerNames = enabled_layers.data();
			}

			VkInstance temp_instance;
			if (auto res = vkCreateInstance(&instance_create_info, nullptr, &temp_instance); res != VK_SUCCESS)
			{
				vvv_throw_not_specified(fmt::format("vkCreateInstance failed with: {}", VkResult_string(res)));
			}
			else
			{
				m_vulkan_instance = std::make_unique<raw_VkInstance>(temp_instance);
			}

			volkLoadInstanceOnly(*m_vulkan_instance);

			//

			VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
			callbackCreateInfo.sType	 = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
			callbackCreateInfo.pNext	 = nullptr;
			callbackCreateInfo.pUserData = this;
			callbackCreateInfo.flags =
				VK_DEBUG_REPORT_INFORMATION_BIT_EXT | VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
			callbackCreateInfo.pfnCallback = [](VkDebugReportFlagsEXT	   flags,
												VkDebugReportObjectTypeEXT objectType,
												uint64_t				   object,
												size_t					   location,
												int32_t					   messageCode,
												const char*				   pLayerPrefix,
												const char*				   pMessage,
												void*					   pUserData) -> VkBool32
			{
				::puts(pMessage);
				return VK_FALSE;
			};

			VkDebugReportCallbackEXT temp_debug_callback{nullptr};
			if (auto res = vkCreateDebugReportCallbackEXT(*m_vulkan_instance, &callbackCreateInfo, nullptr, &temp_debug_callback); res != VK_SUCCESS)
			{
				vvv_throw_not_specified(fmt::format("vkCreateDebugReportCallbackEXT failed with: {}", VkResult_string(res)));
			}
			m_instance_debug_callback = std::make_unique<raw_VkDebugReportCallbackEXT>(*m_vulkan_instance, temp_debug_callback);
		}

		~instance()
		{
			m_instance_debug_callback.reset();
			m_vulkan_instance.reset();
		}

		template<typename T_FUN>
		result<std::vector<std::unique_ptr<physical_device_info>>> discover_physical_devices(T_FUN filter_fun)
		{
			uint32_t num_devices = 0;
			if (auto ret = vkEnumeratePhysicalDevices(*m_vulkan_instance, &num_devices, nullptr); ret != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Failed to count physical devices: {}", VkResult_string(ret)));
			}

			if (num_devices == 0)
			{
				return vvv_err_not_specified("No physical devices found.");
			}

			std::vector<VkPhysicalDevice> physical_devices(num_devices);
			if (auto ret = vkEnumeratePhysicalDevices(*m_vulkan_instance, &num_devices, &physical_devices[0]); ret != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Failed to enumerate physical devices: {}", VkResult_string(ret)));
			}

			std::vector<std::unique_ptr<physical_device_info>> physical_device_infos;
			physical_device_infos.reserve(num_devices);

			for (uint32_t i = 0; i < num_devices; ++i)
			{
				leaf::try_catch(
					[&]()
					{
						auto new_dev = std::make_unique<physical_device_info>(physical_devices[i]);
						if (filter_fun(*new_dev))
						{
							physical_device_infos.push_back(std::move(new_dev));
						}
					},
					[](runtime_error::not_specified err)
					{
						// TODO: spdlog::error(err.m_description);
					});
			}

			if (physical_device_infos.size() > 0)
			{
				return physical_device_infos;
			}
			else
			{
				return vvv_err_not_specified("No valid physical devices found.");
			}
		}

		result<std::vector<std::unique_ptr<physical_device_info>>> discover_all_physical_devices()
		{
			return discover_physical_devices(
				[](const physical_device_info&)
				{
					return true;
				});
		}

		result<std::vector<std::unique_ptr<physical_device_info>>> discover_suitable_physical_devices()
		{
			vvv_LEAF_AUTO(
				physical_devices,
				discover_physical_devices(
					[](const physical_device_info& dev)
					{
						return dev.m_device_features.geometryShader && dev.m_device_features.tessellationShader && dev.m_device_features.sparseBinding;
					}));

			if (physical_devices.size() == 0)
			{
				return vvv_err_not_specified("Unable to discover any valid devices");
			}

			auto score_device_type = [](const auto& props) -> int
			{
				switch (props.deviceType)
				{
				case VK_PHYSICAL_DEVICE_TYPE_OTHER:
					return 0;
				case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
					return 2;
				case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
					return 3;
				case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
					return 0;
				case VK_PHYSICAL_DEVICE_TYPE_CPU:
					return 1;
				default:
					break;
				};
				return 0;
			};

			auto get_device_memory = [](const auto& props) -> int
			{
				size_t total = 0;
				for (int i = 0; i < props.memoryHeapCount; ++i)
				{
					if (props.memoryHeaps[i].flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
					{
						total += props.memoryHeaps[i].size;
					}
				}
				return total;
			};

			std::sort(
				physical_devices.begin(),
				physical_devices.end(),
				[&](std::unique_ptr<physical_device_info>& l, std::unique_ptr<physical_device_info>& r)
				{
					auto type_score = score_device_type(l->m_device_properties);
					if (l->m_device_properties.deviceType == r->m_device_properties.deviceType)
					{
						if (auto [l_mem, r_mem] = std::tuple{get_device_memory(l->m_device_memory_properties), get_device_memory(r->m_device_memory_properties)}; l_mem == r_mem)
						{
							if (auto [l_valid, r_valid] = std::tuple{l->m_device_id_properties.deviceLUIDValid, r->m_device_id_properties.deviceLUIDValid}; l_valid && r_valid)
							{
								const auto* l_uuid = l->m_device_id_properties.deviceUUID;
								const auto* r_uuid = r->m_device_id_properties.deviceUUID;
								for (size_t i = 0; i < sizeof(l->m_device_id_properties.deviceUUID); ++i)
								{
									if (l_uuid[i] != r_uuid[i])
									{
										return l_uuid[i] > r_uuid[i];
									}
								}
							}

							const auto* l_luid = l->m_device_id_properties.deviceLUID;
							const auto* r_luid = r->m_device_id_properties.deviceLUID;
							for (size_t i = 0; i < sizeof(l->m_device_id_properties.deviceLUID); ++i)
							{
								if (l_luid[i] != r_luid[i])
								{
									return l_luid[i] > r_luid[i];
								}
							}

							vvv_throw_not_specified("Unable to discern devices");
						}
						else
						{
							return l_mem > r_mem;
						}
					}
					else
					{
						return type_score > score_device_type(r->m_device_properties);
					}
				});

			return physical_devices;
		}

		// TODO: get sorted devices for a given window/HWND

		result<std::unique_ptr<physical_device>> get_default_physical_device()
		{
			vvv_LEAF_AUTO(physical_devices, discover_suitable_physical_devices());
			return std::make_unique<vvv::physical_device>(std::move(physical_devices[0]));
		}

#if defined(VK_KHR_win32_surface)
		result<std::unique_ptr<surface>> create_surface(HWND hwnd, HINSTANCE hinst)
		{
			VkWin32SurfaceCreateInfoKHR createInfo{};
			createInfo.sType	 = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			createInfo.hwnd		 = hwnd;
			createInfo.hinstance = hinst;

			VkSurfaceKHR surf;
			if (auto res = vkCreateWin32SurfaceKHR(*m_vulkan_instance, &createInfo, nullptr, &surf); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("vkCreateWin32SurfaceKHR failed: {}", VkResult_string(res)));
			}

			return std::make_unique<surface>(surf);
		}
#endif
	};

} // namespace vvv
