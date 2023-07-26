#pragma once

#include "vvv_base.h"

namespace vvv
{
	class raw_VkApplicationInfo
	{
	protected:
		friend class raw_VkApplication;

		VkApplicationInfo m_value{};

		std::string m_application_name;
		std::string m_engine_name;

		int	   m_argc{0};
		char** m_argv{nullptr};

	public:
		raw_VkApplicationInfo()
		{
			m_value.sType	   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			m_value.apiVersion = VK_API_VERSION_1_3;
		}

		virtual ~raw_VkApplicationInfo() { }

		auto& with_cmdline(int argc, char** argv)
		{
			m_argc = argc;
			m_argv = argv;
			return *this;
		}

		auto& with_defaults()
		{
			m_value.pApplicationName   = "Loopunit";
			m_value.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
			m_value.pEngineName		   = "vvv";
			m_value.engineVersion	   = VK_MAKE_VERSION(0, 0, 1);
			return *this;
		}

		auto& with_application_name(std::string&& name)
		{
			m_application_name		 = std::move(name);
			m_value.pApplicationName = m_application_name.c_str();
			return *this;
		}

		auto& with_application_name(const char* name)
		{
			m_application_name		 = name;
			m_value.pApplicationName = m_application_name.c_str();
			return *this;
		}

		auto& with_application_version(uint32_t applicationVersion)
		{
			m_value.applicationVersion = applicationVersion;
			return *this;
		}

		auto& with_engine_name(std::string&& name)
		{
			m_engine_name		= std::move(name);
			m_value.pEngineName = m_engine_name.c_str();
			return *this;
		}

		auto& with_engine_name(const char* name)
		{
			m_engine_name		= name;
			m_value.pEngineName = m_engine_name.c_str();
			return *this;
		}

		auto& with_engine_version(uint32_t engineVersion)
		{
			m_value.engineVersion = engineVersion;
			return *this;
		}

		auto& with_api_version(uint32_t apiVersion)
		{
			m_value.apiVersion = apiVersion;
			return *this;
		}

		auto* data()
		{
			return &m_value;
		}
	};

	class raw_VkApplication
	{
	protected:
		VkApplicationInfo			 m_app_info{};
		const VkAllocationCallbacks* m_allocation_callbacks{nullptr};
		VkAllocationCallbacks		 m_custom_allocation_callbacks;

	public:
		raw_VkApplication(VkApplicationInfo app_info)
			: m_app_info{app_info}
			, m_custom_allocation_callbacks{
				  .pUserData	 = this,
				  .pfnAllocation = [](void* pUserData, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) -> void*
				  {
					  return nullptr;
				  },
				  .pfnReallocation = [](void* pUserData, void* pOriginal, size_t size, size_t alignment, VkSystemAllocationScope allocationScope) -> void*
				  {
					  return nullptr;
				  },
				  .pfnFree				 = [](void* pUserData, void* pMemory) {},
				  .pfnInternalAllocation = [](void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {},
				  .pfnInternalFree		 = [](void* pUserData, size_t size, VkInternalAllocationType allocationType, VkSystemAllocationScope allocationScope) {}}
		{ }

		virtual ~raw_VkApplication() = default;

		auto allocation_callbacks() const
		{
			return m_allocation_callbacks;
		}

		auto* get_vk_app_info()
		{
			return &m_app_info;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::unique_ptr<raw_VkApplicationInfo> appinfo)
		{
			::_wputenv_s(L"VK_LAYER_PATH", std::filesystem::path(appinfo->m_argv[0]).parent_path().c_str());

			if (auto r = volkInitialize(); r != VK_SUCCESS)
			{
				return vvv_err_not_specified("volkInitialize failed!");
			}

			// auto version = volkGetInstanceVersion();
			// printf("Vulkan version %d.%d.%d initialized.\n", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));

			return std::make_unique<T_IMPL>(*(appinfo->data()));
		}
	};
} // namespace vvv

namespace vvv
{
	class raw_VkInstanceCreateInfo
	{
	protected:
		friend class raw_VkInstance;

		VkInstanceCreateInfo m_value{};

		std::shared_ptr<raw_VkApplication> m_vk;
		std::vector<const char*>		   m_enabled_layers;
		std::vector<const char*>		   m_enabled_extensions;

	public:
		raw_VkInstanceCreateInfo(std::shared_ptr<raw_VkApplication> vk) : m_vk{vk}
		{
			m_value.sType			 = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			m_value.pApplicationInfo = m_vk->get_vk_app_info();
		}

		virtual ~raw_VkInstanceCreateInfo() { }

		auto& with_flags(VkInstanceCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_layers(std::vector<const char*>&& enabled_layers)
		{
			m_enabled_layers			= std::move(enabled_layers);
			m_value.enabledLayerCount	= static_cast<uint32_t>(m_enabled_layers.size());
			m_value.ppEnabledLayerNames = m_enabled_layers.data();
			return *this;
		}

		static inline std::vector<const char*> filter_layers(std::vector<const char*> desired_layers)
		{
			uint32_t availiable_layer_count = 0;
			vkEnumerateInstanceLayerProperties(&availiable_layer_count, nullptr);
			std::vector<VkLayerProperties> available_layers(availiable_layer_count);
			vkEnumerateInstanceLayerProperties(&availiable_layer_count, available_layers.data());

			std::vector<const char*> enabled_layers;

			for (auto ext_str : desired_layers)
			{
				for (auto& ext : available_layers)
				{
					if (::stricmp(ext.layerName, ext_str) == 0)
					{
						enabled_layers.push_back(ext_str);
						break;
					}
				}
			}

			return enabled_layers;
		}

		static inline std::vector<const char*> filter_extensions(std::vector<const char*> desired_extensions)
		{
			uint32_t availiable_extension_count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &availiable_extension_count, nullptr);
			std::vector<VkExtensionProperties> available_extensions(availiable_extension_count);
			vkEnumerateInstanceExtensionProperties(nullptr, &availiable_extension_count, available_extensions.data());

			std::vector<const char*> enabled_extensions;

			for (auto ext_str : desired_extensions)
			{
				for (auto& ext : available_extensions)
				{
					if (::stricmp(ext.extensionName, ext_str) == 0)
					{
						enabled_extensions.push_back(ext_str);
						break;
					}
				}
			}

			return enabled_extensions;
		}

		auto& with_default_layers()
		{
			auto enabled_layers = filter_layers({
				"VK_LAYER_KHRONOS_validation",
				"VK_LAYER_NV_optimus",
			});

			return with_layers(std::move(enabled_layers));
		}

		auto& with_extensions(std::vector<const char*>&& enabled_extensions)
		{
			m_enabled_extensions			= std::move(enabled_extensions);
			m_value.enabledExtensionCount	= static_cast<uint32_t>(m_enabled_extensions.size());
			m_value.ppEnabledExtensionNames = m_enabled_extensions.data();
			return *this;
		}

		auto& with_default_extensions()
		{
			auto enabled_extensions = filter_extensions({
				VK_KHR_SURFACE_EXTENSION_NAME, VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME, VK_EXT_DEBUG_REPORT_EXTENSION_NAME, VK_KHR_DISPLAY_EXTENSION_NAME,
					VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME, VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME, VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
					VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
					VK_KHR_WIN32_SURFACE_EXTENSION_NAME, VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME, VK_KHR_WIN32_KEYED_MUTEX_EXTENSION_NAME,
					VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME, VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME,
				/* VK_NV_ACQUIRE_WINRT_DISPLAY_EXTENSION_NAME,*/
#elif defined(VK_USE_PLATFORM_METAL_EXT)
					VK_EXT_METAL_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XCB_KHR)
					VK_KHR_XCB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_XLIB_KHR)
					VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WAYLAND_KHR)
					VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_DISPLAY_KHR)
					VK_KHR_DISPLAY_EXTENSION_NAME,
#else
#pragma error Platform not supported
#endif
			});

			return with_extensions(std::move(enabled_extensions));
		}

		auto* data()
		{
			return &m_value;
		}

		auto get_application()
		{
			return m_vk;
		}
	};

	class raw_VkInstance
	{
	protected:
		VkInstance m_instance{nullptr};

		std::shared_ptr<raw_VkApplication> m_vk;
		VkDebugReportCallbackEXT		   m_debug_report_callbacks{nullptr};

	public:
		raw_VkInstance(std::shared_ptr<raw_VkApplication> vk, VkInstance inst, VkDebugReportCallbackEXT cb) : m_vk{vk}, m_instance{inst}, m_debug_report_callbacks{cb} { }

		virtual ~raw_VkInstance()
		{
			if (m_debug_report_callbacks)
			{
				vkDestroyDebugReportCallbackEXT(m_instance, m_debug_report_callbacks, m_vk->allocation_callbacks());
			}

			if (m_instance)
			{
				vkDestroyInstance(m_instance, m_vk->allocation_callbacks());
			}
		}

		auto allocation_callbacks() const
		{
			return m_vk->allocation_callbacks();
		}

		auto get_vk_instance() const
		{
			return m_instance;
		}

		template<typename T_FUN>
		result<std::vector<VkPhysicalDeviceCreateInfo>> discover_physical_devices(T_FUN filter_fun)
		{
			uint32_t num_devices = 0;
			if (auto ret = vkEnumeratePhysicalDevices(m_instance, &num_devices, nullptr); ret != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Failed to count physical devices: {}", VkResult_string(ret)));
			}

			if (num_devices == 0)
			{
				return vvv_err_not_specified("No physical devices found.");
			}

			std::vector<VkPhysicalDevice> physical_devices(num_devices);
			if (auto ret = vkEnumeratePhysicalDevices(m_instance, &num_devices, &physical_devices[0]); ret != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Failed to enumerate physical devices: {}", VkResult_string(ret)));
			}

			std::vector<VkPhysicalDeviceCreateInfo> filtered_physical_devices;
			for (uint32_t i = 0; i < num_devices; ++i)
			{
				VkPhysicalDeviceCreateInfo info;
				info.physicalDevice = physical_devices[i];

				vkGetPhysicalDeviceProperties(physical_devices[i], &info.physicalDeviceProperties);
				vkGetPhysicalDeviceFeatures(physical_devices[i], &info.physicalDeviceFeatures);

				VkPhysicalDeviceProperties2 physDeviceProps = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
				physDeviceProps.pNext						= &info.physicalDeviceIDProperties;
				vkGetPhysicalDeviceProperties2(physical_devices[i], &physDeviceProps);

				vkGetPhysicalDeviceMemoryProperties(physical_devices[i], &info.physicalDeviceMemoryProperties);

				if (filter_fun(info))
				{
					filtered_physical_devices.push_back(info);
				}
			}

			if (filtered_physical_devices.size() > 0)
			{
				return filtered_physical_devices;
			}
			else
			{
				return vvv_err_not_specified("No valid physical devices found.");
			}
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::unique_ptr<raw_VkInstanceCreateInfo> ci)
		{
			auto vk = ci->get_application();

			vvv_LEAF_AUTO(
				new_instance,
				[&]() -> result<VkInstance>
				{
					VkInstance new_instance{nullptr};
					if (auto res = vkCreateInstance(ci->data(), vk->allocation_callbacks(), &new_instance); res != VK_SUCCESS)
					{
						return vvv_err_not_specified(fmt::format("vkCreateInstance failed with: {}", VkResult_string(res)));
					}
					else
					{
						return new_instance;
					}
				}());

			volkLoadInstanceOnly(new_instance);

			vvv_CLEAF_AUTO(
				debug_report_callbacks,
				[&]() -> result<VkDebugReportCallbackEXT>
				{
					VkDebugReportCallbackEXT debug_report_callbacks;

					VkDebugReportCallbackCreateInfoEXT callbackCreateInfo;
					callbackCreateInfo.sType	 = VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT;
					callbackCreateInfo.pNext	 = nullptr;
					callbackCreateInfo.pUserData = nullptr;
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

					if (auto res = vkCreateDebugReportCallbackEXT(new_instance, &callbackCreateInfo, vk->allocation_callbacks(), &debug_report_callbacks); res != VK_SUCCESS)
					{
						return vvv_err_not_specified(fmt::format("vkCreateDebugReportCallbackEXT failed with: {}", VkResult_string(res)));
					}

					return debug_report_callbacks;
				}(),
				[&]()
				{
					vkDestroyInstance(new_instance, vk->allocation_callbacks());
				});

			return std::make_unique<T_IMPL>(vk, new_instance, debug_report_callbacks);
		}
	};
} // namespace vvv

namespace vvv
{
	class raw_VkPhysicalDeviceCreateInfo
	{
	protected:
		std::shared_ptr<raw_VkInstance> m_instance;
		VkPhysicalDeviceCreateInfo		m_value;

	public:
		raw_VkPhysicalDeviceCreateInfo(std::shared_ptr<raw_VkInstance> instance) : m_instance{instance} { }

		virtual ~raw_VkPhysicalDeviceCreateInfo() { }

		auto& with_device_info(const VkPhysicalDeviceCreateInfo& physicalDeviceInfo)
		{
			m_value = physicalDeviceInfo;
			return *this;
		}

		auto* data()
		{
			return &m_value;
		}

		auto get_instance()
		{
			return m_instance;
		}
	};

	class raw_VkPhysicalDevice
	{
	protected:
		std::shared_ptr<raw_VkInstance>		m_vk_instance;
		VkPhysicalDevice					m_physical_device{nullptr};
		VkPhysicalDeviceProperties			m_device_properties{};
		VkPhysicalDeviceFeatures			m_device_features{};
		VkPhysicalDeviceIDProperties		m_device_id_properties{};
		VkPhysicalDeviceMemoryProperties	m_device_memory_properties{};
		std::vector<VkDisplayPropertiesKHR> m_ext_display_properties; // Displays that are NOT connected to the desktop

		result<void> init()
		{
			m_device_id_properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES;

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

			return {};
		}

	public:
		raw_VkPhysicalDevice(std::shared_ptr<raw_VkInstance> vk_instance, VkPhysicalDeviceCreateInfo physicalDeviceInfo)
			: m_vk_instance{vk_instance}
			, m_physical_device{physicalDeviceInfo.physicalDevice}
		{ }

		virtual ~raw_VkPhysicalDevice() { }

		result<std::vector<VkQueueFamilyProperties>> get_queue_family_properties()
		{
			uint32_t count = 0;
			if (vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &count, nullptr); count > 0)
			{
				std::vector<VkQueueFamilyProperties> queue_family_props(count);

				vkGetPhysicalDeviceQueueFamilyProperties(m_physical_device, &count, queue_family_props.data());
				return queue_family_props;
			}
			else
			{
				return vvv_err_not_specified("Unable to discover any valid devices");
			}
		}

		std::vector<const char*> filter_extensions(std::vector<const char*> desired_extensions)
		{
			uint32_t extension_count = 0;
			vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, nullptr);

			std::vector<VkExtensionProperties> available_extensions(extension_count);
			vkEnumerateDeviceExtensionProperties(m_physical_device, nullptr, &extension_count, available_extensions.data());

			std::vector<const char*> enabled_extensions;

			for (auto ext_str : desired_extensions)
			{
				for (auto& ext : available_extensions)
				{
					if (::stricmp(ext.extensionName, ext_str) == 0)
					{
						enabled_extensions.push_back(ext_str);
						break;
					}
				}
			}

			return enabled_extensions;
		}

		auto get_vk_physical_device() const
		{
			return m_physical_device;
		}

		auto get_vk_instance() const
		{
			return m_vk_instance->get_vk_instance();
		}

		auto allocation_callbacks() const
		{
			return m_vk_instance->allocation_callbacks();
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::unique_ptr<raw_VkPhysicalDeviceCreateInfo> ci)
		{
			auto result = std::make_unique<T_IMPL>(ci->get_instance(), *ci->data());
			vvv_LEAF_CHECK(result->init());
			return result;
		}
	};
} // namespace vvv

namespace vvv
{
	class raw_VkSurface
	{
	protected:
		std::shared_ptr<raw_VkInstance> m_instance;
		VkSurfaceKHR					m_surface;

	public:
		raw_VkSurface(std::shared_ptr<raw_VkInstance> instance, VkSurfaceKHR surface) : m_instance{instance}, m_surface{surface} { }

		virtual ~raw_VkSurface()
		{
			if (m_surface)
			{
				vkDestroySurfaceKHR(m_instance->get_vk_instance(), m_surface, m_instance->allocation_callbacks());
			}
		}

		auto get_vk_surface()
		{
			return m_surface;
		}

		result<vk_surface_capabilities> get_surface_capabilities(raw_VkPhysicalDevice& physical_dev)
		{
			vk_surface_capabilities cap;
			if (auto res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_dev.get_vk_physical_device(), m_surface, &cap.m_capabilities); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface caps: {}", VkResult_string(res)));
			}

			uint32_t format_count{0};
			if (auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_dev.get_vk_physical_device(), m_surface, &format_count, nullptr); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface formats: {}", VkResult_string(res)));
			}

			cap.m_formats.resize(format_count);
			if (auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_dev.get_vk_physical_device(), m_surface, &format_count, cap.m_formats.data()); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface formats: {}", VkResult_string(res)));
			}

			uint32_t present_mode_count{0};
			if (auto res = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_dev.get_vk_physical_device(), m_surface, &present_mode_count, nullptr); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface present modes: {}", VkResult_string(res)));
			}

			cap.m_present_modes.resize(present_mode_count);
			if (auto res = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_dev.get_vk_physical_device(), m_surface, &present_mode_count, cap.m_present_modes.data());
				res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface present modes: {}", VkResult_string(res)));
			}

			return cap;
		}

#if defined(VK_KHR_win32_surface)
		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_VkInstance> instance, HWND hwnd)
		{
			VkWin32SurfaceCreateInfoKHR create_info{};
			create_info.sType	  = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			create_info.hwnd	  = hwnd;
			create_info.hinstance = ::GetModuleHandleA(nullptr);

			VkSurfaceKHR surface{nullptr};
			if (auto res = vkCreateWin32SurfaceKHR(instance->get_vk_instance(), &create_info, instance->allocation_callbacks(), &surface); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to create surface: {}", VkResult_string(res)));
			}

			return std::make_unique<T_IMPL>(instance, surface);
		}
#endif
	};
} // namespace vvv

namespace vvv
{
	class raw_VkQueueFamiliesCreateInfo
	{
	protected:
		std::shared_ptr<raw_VkPhysicalDevice> m_physical_device;

	public:
		raw_VkQueueFamiliesCreateInfo(std::shared_ptr<raw_VkPhysicalDevice> physical_device) : m_physical_device{physical_device} { }
		virtual ~raw_VkQueueFamiliesCreateInfo() { }

		auto get_physical_device()
		{
			return m_physical_device;
		}
	};

	class raw_VkQueueFamilies
	{
	protected:
		std::shared_ptr<raw_VkPhysicalDevice> m_physical_device;
		std::vector<VkQueueFamilyProperties>  m_family_properties;

	public:
		raw_VkQueueFamilies(std::shared_ptr<raw_VkPhysicalDevice> physical_device, std::vector<VkQueueFamilyProperties>&& family_properties)
			: m_physical_device{physical_device}
			, m_family_properties{family_properties}
		{ }

		virtual ~raw_VkQueueFamilies() { }

		auto& get_family_properties()
		{
			return m_family_properties;
		}

		static inline result<std::vector<VkQueueFamilyProperties>> get_queue_family_properties(VkPhysicalDevice phys_dev)
		{
			uint32_t count = 0;
			if (vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &count, nullptr); count > 0)
			{
				std::vector<VkQueueFamilyProperties> queue_family_props(count);

				vkGetPhysicalDeviceQueueFamilyProperties(phys_dev, &count, queue_family_props.data());
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

		result<vk_queue_family_mapping> find_exclusive_queue_families(/*VkSurfaceKHR surface*/)
		{
			vvv_LEAF_AUTO(props, get_queue_family_properties(m_physical_device->get_vk_physical_device()));

			vk_queue_family_mapping out_mappings{vk_queue_family_mapping_init};

			auto find_exclusive_family = [&](auto type_bits)
			{
				for (uint32_t i = 0; i < static_cast<uint32_t>(props.size()); ++i)
				{
					if (get_queue_family_support(props[i], type_bits) && ((type_bits == VK_QUEUE_GRAPHICS_BIT) || (i != out_mappings[enum_value(vk_queue_family_type::GFX)])))
					{
						return i;
					}
				}
				return vk_queue_family_mapping_invalid;
			};

			out_mappings[enum_value(vk_queue_family_type::GFX)]			 = find_exclusive_family(VK_QUEUE_GRAPHICS_BIT);
			out_mappings[enum_value(vk_queue_family_type::TRANSFER)]	 = find_exclusive_family(VK_QUEUE_TRANSFER_BIT);
			out_mappings[enum_value(vk_queue_family_type::COMPUTE)]		 = find_exclusive_family(VK_QUEUE_COMPUTE_BIT);
			out_mappings[enum_value(vk_queue_family_type::VIDEO_DECODE)] = find_exclusive_family(VK_QUEUE_VIDEO_DECODE_BIT_KHR);
			out_mappings[enum_value(vk_queue_family_type::VIDEO_ENCODE)] = find_exclusive_family(VK_QUEUE_VIDEO_ENCODE_BIT_KHR);

			// Check for present support on the gfx and present queue
			std::array<VkBool32, enum_value(vk_queue_family_type::COUNT)> present_supported;
			for (size_t i = 0; i < out_mappings.size(); ++i)
			{
				if (out_mappings[i] != vk_queue_family_mapping_invalid)
				{
#if defined(VK_KHR_win32_surface)
					present_supported[i] = vkGetPhysicalDeviceWin32PresentationSupportKHR(m_physical_device->get_vk_physical_device(), out_mappings[i]);
#else
#error Unsupported
#endif
					// TODO: This seems to be implied if presentation support exists.
					// if (auto res = vkGetPhysicalDeviceSurfaceSupportKHR(m_physical_device->get_vk_physical_device(), out_mappings[i], surface, &present_supported[i]);
					//	res != VK_SUCCESS) [[unlikely]]
					//{
					//	return vvv_err_not_specified(fmt::format("Unable get queue ({}) support for surface: {}", i, VkResult_string(res)));
					// }
				}
				else
				{
					present_supported[i] = false;
				}
			}

			auto supports_async_present_queue = [&](uint32_t q_idx)
			{
				auto q_family_idx = out_mappings[q_idx];
				if (q_family_idx != vk_queue_family_mapping_invalid)
				{
					auto& q_prop = props[q_family_idx];
					return ((q_prop.queueCount > 1) && present_supported[q_idx]);
				}
				return false;
			};

			if (supports_async_present_queue(enum_value(vk_queue_family_type::COMPUTE)))
			{
				out_mappings[enum_value(vk_queue_family_type::PRESENT)] = out_mappings[enum_value(vk_queue_family_type::COMPUTE)];
			}
			else if (supports_async_present_queue(enum_value(vk_queue_family_type::GFX)))
			{
				out_mappings[enum_value(vk_queue_family_type::PRESENT)] = out_mappings[enum_value(vk_queue_family_type::GFX)];
			}
			else
			{
				return vvv_err_not_specified(fmt::format("Unable to find async present queue"));
			}

			return out_mappings;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::unique_ptr<raw_VkQueueFamiliesCreateInfo> ci)
		{
			vvv_LEAF_AUTO(devs, get_queue_family_properties(ci->get_physical_device()->get_vk_physical_device()));
			return std::make_unique<T_IMPL>(ci->get_physical_device(), std::move(devs));
		}
	};
} // namespace vvv

namespace vvv
{
	class raw_VkDeviceCreateInfo
	{
	protected:
		std::shared_ptr<raw_VkPhysicalDevice> m_physical_device;
		// std::shared_ptr<raw_VkQueueFamilies>  m_queue_families;

		VkDeviceCreateInfo					 m_value{};
		VkPhysicalDeviceFeatures			 m_device_features{};
		std::vector<const char*>			 m_enabled_extensions;
		std::vector<VkDeviceQueueCreateInfo> m_queue_families_ci;
		std::vector<float>					 m_queue_families_ci_prio;

	public:
		raw_VkDeviceCreateInfo(std::shared_ptr<raw_VkPhysicalDevice> physical_device /*, std::shared_ptr<raw_VkQueueFamilies> queue_families*/) : m_physical_device{physical_device}
		//, m_queue_families{queue_families}
		{
			m_value.sType			 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			m_value.pEnabledFeatures = &m_device_features;

			// Device layers are deprecated and ignored
			m_value.enabledLayerCount	= 0;
			m_value.ppEnabledLayerNames = nullptr;
		}

		virtual ~raw_VkDeviceCreateInfo() { }

		auto& with_features(VkPhysicalDeviceFeatures f)
		{
			m_device_features = f;
			return *this;
		}

		auto& with_extensions(std::vector<const char*>&& enabled_extensions)
		{
			m_enabled_extensions = std::move(enabled_extensions);
			if (m_enabled_extensions.size() > 0)
			{
				m_value.enabledExtensionCount	= static_cast<uint32_t>(m_enabled_extensions.size());
				m_value.ppEnabledExtensionNames = m_enabled_extensions.data();
			}
			return *this;
		}

		auto& with_queue_families(std::vector<VkDeviceQueueCreateInfo>&& queue_families_ci, std::vector<float>&& queue_families_ci_prio)
		{
			m_queue_families_ci		 = std::move(queue_families_ci);
			m_queue_families_ci_prio = std::move(queue_families_ci_prio);

			m_value.pQueueCreateInfos	 = m_queue_families_ci.data();
			m_value.queueCreateInfoCount = m_queue_families_ci.size();

			return *this;
		}

		auto* data()
		{
			return &m_value;
		}

		auto get_physical_device()
		{
			return m_physical_device;
		}
	};

	class raw_VkDevice
	{
	protected:
		std::shared_ptr<raw_VkPhysicalDevice> m_physical_device;
		VkDevice							  m_vk_device{nullptr};
		VmaAllocator						  m_allocator;
		VolkDeviceTable						  m_volk_device_table;

	public:
		raw_VkDevice(std::shared_ptr<raw_VkPhysicalDevice> physical_device, VkDevice vk_device, VmaAllocator allocator, VolkDeviceTable volk_device_table)
			: m_physical_device{physical_device}
			, m_vk_device{vk_device}
			, m_allocator{allocator}
			, m_volk_device_table{volk_device_table}
		{ }

		virtual ~raw_VkDevice()
		{
			if (m_vk_device)
			{
				m_volk_device_table.vkDestroyDevice(m_vk_device, m_physical_device->allocation_callbacks());
			}
		}

		const VolkDeviceTable& device_table() const
		{
			return m_volk_device_table;
		}

		auto allocation_callbacks() const
		{
			return m_physical_device->allocation_callbacks();
		}

		auto get_vk_device() const
		{
			return m_vk_device;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::unique_ptr<raw_VkDeviceCreateInfo> create_info)
		{
			VkDevice device{nullptr};
			auto	 physical_device = create_info->get_physical_device();

			if (auto res = vkCreateDevice(physical_device->get_vk_physical_device(), create_info->data(), physical_device->allocation_callbacks(), &device); res != VK_SUCCESS)
				[[unlikely]]
			{
				return vvv_err_not_specified(fmt::format("Unable to create logical device: {}", VkResult_string(res)));
			}

			VolkDeviceTable vdt;
			volkLoadDeviceTable(&vdt, device);

			VmaVulkanFunctions vma_funcs;

			vma_funcs.vkGetPhysicalDeviceProperties		  = vkGetPhysicalDeviceProperties;
			vma_funcs.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
			vma_funcs.vkAllocateMemory					  = vdt.vkAllocateMemory;
			vma_funcs.vkFreeMemory						  = vdt.vkFreeMemory;
			vma_funcs.vkMapMemory						  = vdt.vkMapMemory;
			vma_funcs.vkUnmapMemory						  = vdt.vkUnmapMemory;
			vma_funcs.vkFlushMappedMemoryRanges			  = vdt.vkFlushMappedMemoryRanges;
			vma_funcs.vkInvalidateMappedMemoryRanges	  = vdt.vkInvalidateMappedMemoryRanges;
			vma_funcs.vkBindBufferMemory				  = vdt.vkBindBufferMemory;
			vma_funcs.vkBindImageMemory					  = vdt.vkBindImageMemory;
			vma_funcs.vkGetBufferMemoryRequirements		  = vdt.vkGetBufferMemoryRequirements;
			vma_funcs.vkGetImageMemoryRequirements		  = vdt.vkGetImageMemoryRequirements;
			vma_funcs.vkCreateBuffer					  = vdt.vkCreateBuffer;
			vma_funcs.vkDestroyBuffer					  = vdt.vkDestroyBuffer;
			vma_funcs.vkCreateImage						  = vdt.vkCreateImage;
			vma_funcs.vkDestroyImage					  = vdt.vkDestroyImage;
			vma_funcs.vkCmdCopyBuffer					  = vdt.vkCmdCopyBuffer;
#if VMA_DEDICATED_ALLOCATION || VMA_VULKAN_VERSION >= 1001000
			vma_funcs.vkGetBufferMemoryRequirements2KHR = vdt.vkGetBufferMemoryRequirements2KHR;
			vma_funcs.vkGetImageMemoryRequirements2KHR	= vdt.vkGetImageMemoryRequirements2KHR;
#endif
#if VMA_BIND_MEMORY2 || VMA_VULKAN_VERSION >= 1001000
			vma_funcs.vkBindBufferMemory2KHR = vdt.vkBindBufferMemory2KHR;
			vma_funcs.vkBindImageMemory2KHR	 = vdt.vkBindImageMemory2KHR;
#endif
#if VMA_MEMORY_BUDGET || VMA_VULKAN_VERSION >= 1001000

			vma_funcs.vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR;
#endif
#if VMA_VULKAN_VERSION >= 1003000
			vma_funcs.vkGetDeviceBufferMemoryRequirements = vdt.vkGetDeviceBufferMemoryRequirements;
			vma_funcs.vkGetDeviceImageMemoryRequirements  = vdt.vkGetDeviceImageMemoryRequirements;
#endif

			VmaAllocatorCreateInfo allocatorCreateInfo = {};
			allocatorCreateInfo.vulkanApiVersion	   = VK_API_VERSION_1_3;
			allocatorCreateInfo.physicalDevice		   = physical_device->get_vk_physical_device();
			allocatorCreateInfo.device				   = device;
			allocatorCreateInfo.instance			   = physical_device->get_vk_instance();
			allocatorCreateInfo.pVulkanFunctions	   = &vma_funcs;

			VmaAllocator allocator;
			if (auto res = vmaCreateAllocator(&allocatorCreateInfo, &allocator); res != VK_SUCCESS) [[unlikely]]
			{
				vdt.vkDestroyDevice(device, create_info->get_physical_device()->allocation_callbacks());
				return vvv_err_not_specified(fmt::format("vmaCreateAllocatorfailed with: {}", VkResult_string(res)));
			}

			return std::make_unique<T_IMPL>(physical_device, device, allocator, vdt);
		}
	};
} // namespace vvv

namespace vvv
{
	class raw_VkSwapchainCreateInfo
	{
	protected:
		friend class raw_VkSwapchain;

		std::shared_ptr<raw_VkDevice>  m_device;
		std::shared_ptr<raw_VkSurface> m_surface;

		VkExtent2D m_swapchain_size;

		VkSwapchainCreateInfoKHR m_value{};

	public:
		raw_VkSwapchainCreateInfo(std::shared_ptr<raw_VkDevice> device) : m_device{device}
		{
			m_value.sType				  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			m_value.imageArrayLayers	  = 1;
			m_value.imageUsage			  = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			m_value.imageSharingMode	  = VK_SHARING_MODE_EXCLUSIVE;
			m_value.queueFamilyIndexCount = 0;
			m_value.pQueueFamilyIndices	  = nullptr;
			m_value.clipped				  = VK_TRUE;
			m_value.oldSwapchain		  = VK_NULL_HANDLE;
		}

		virtual ~raw_VkSwapchainCreateInfo() { }

		auto& with_surface(std::shared_ptr<raw_VkSurface> surface)
		{
			m_surface		= surface;
			m_value.surface = m_surface->get_vk_surface();
			return *this;
		}

		auto& with_settings(
			uint32_t					  image_count,
			VkFormat					  surface_format,
			VkColorSpaceKHR				  color_space,
			VkSurfaceTransformFlagBitsKHR transform,
			VkPresentModeKHR			  present_mode,
			VkCompositeAlphaFlagBitsKHR	  composite)
		{
			m_value.minImageCount	= image_count;
			m_value.imageFormat		= surface_format;
			m_value.imageColorSpace = color_space;
			m_value.preTransform	= transform;
			m_value.compositeAlpha	= composite;
			m_value.presentMode		= present_mode;

			return *this;
		}

		auto& with_size(VkExtent2D swapchain_size)
		{
			m_value.imageExtent = swapchain_size;
			return *this;
		}

		auto* data()
		{
			return &m_value;
		}
	};

	class raw_VkSwapchain
	{
	protected:
		std::shared_ptr<raw_VkDevice>  m_device;
		std::shared_ptr<raw_VkSurface> m_surface;
		VkSwapchainKHR				   m_swapchain{nullptr};
		VkSwapchainCreateInfoKHR	   m_create_info;
		std::vector<VkImage>		   m_swapchain_images;
		std::vector<VkImageView>	   m_swapchain_image_views;

	public:
		raw_VkSwapchain(std::shared_ptr<raw_VkDevice> device, std::shared_ptr<raw_VkSurface> surface, VkSwapchainKHR swapchain, VkSwapchainCreateInfoKHR create_info)
			: m_device{device}
			, m_surface{surface}
			, m_swapchain{swapchain}
			, m_create_info{create_info}
		{
			create_info.oldSwapchain = VK_NULL_HANDLE;

			uint32_t image_count{0};
			if (auto res = m_device->device_table().vkGetSwapchainImagesKHR(m_device->get_vk_device(), m_swapchain, &image_count, nullptr); res != VK_SUCCESS)
			{
				vvv_throw_not_specified(fmt::format("Unable get swapchain images {}", VkResult_string(res)));
			}

			m_swapchain_images.resize(image_count);

			if (auto res = m_device->device_table().vkGetSwapchainImagesKHR(m_device->get_vk_device(), m_swapchain, &image_count, m_swapchain_images.data()); res != VK_SUCCESS)
			{
				vvv_throw_not_specified(fmt::format("Unable get swapchain images {}", VkResult_string(res)));
			}

			m_swapchain_image_views.resize(image_count);

			for (auto i = 0u; i < image_count; ++i)
			{
				VkImageViewCreateInfo create_info{};
				create_info.sType							= VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				create_info.image							= m_swapchain_images[i];
				create_info.viewType						= VK_IMAGE_VIEW_TYPE_2D;
				create_info.format							= m_create_info.imageFormat;
				create_info.components.r					= VK_COMPONENT_SWIZZLE_IDENTITY;
				create_info.components.g					= VK_COMPONENT_SWIZZLE_IDENTITY;
				create_info.components.b					= VK_COMPONENT_SWIZZLE_IDENTITY;
				create_info.components.a					= VK_COMPONENT_SWIZZLE_IDENTITY;
				create_info.subresourceRange.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
				create_info.subresourceRange.baseMipLevel	= 0;
				create_info.subresourceRange.levelCount		= 1;
				create_info.subresourceRange.baseArrayLayer = 0;
				create_info.subresourceRange.layerCount		= 1;

				if (auto res = m_device->device_table().vkCreateImageView(m_device->get_vk_device(), &create_info, m_device->allocation_callbacks(), &m_swapchain_image_views[i]);
					res != VK_SUCCESS)
				{
					vvv_throw_not_specified(fmt::format("Unable create swapchain view ({}) {}", i, VkResult_string(res)));
				}
			}
		}

		virtual ~raw_VkSwapchain()
		{
			for (auto& img_view : m_swapchain_image_views)
			{
				if (img_view)
				{
					m_device->device_table().vkDestroyImageView(m_device->get_vk_device(), img_view, m_device->allocation_callbacks());
				}
			}

			if (m_swapchain)
			{
				m_device->device_table().vkDestroySwapchainKHR(m_device->get_vk_device(), m_swapchain, m_device->allocation_callbacks());
			}
		}

		auto get_surface_format() const
		{
			return m_create_info.imageFormat;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::unique_ptr<raw_VkSwapchainCreateInfo> ci)
		{
			auto& device  = ci->m_device;
			auto& surface = ci->m_surface;

			VkSwapchainKHR swapchain{nullptr};
			if (auto res = device->device_table().vkCreateSwapchainKHR(device->get_vk_device(), ci->data(), nullptr, &swapchain); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to create swapchain: {}", VkResult_string(res)));
			}

			return std::make_unique<T_IMPL>(device, surface, swapchain, ci->m_value);
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_VkSwapchain> previous, VkExtent2D swapchain_size)
		{
			VkSwapchainCreateInfoKHR create_info{previous->m_create_info};
			create_info.imageExtent	 = swapchain_size;
			create_info.oldSwapchain = previous->m_swapchain;

			VkSwapchainKHR swapchain{nullptr};
			if (auto res = previous->m_device->device_table().vkCreateSwapchainKHR(previous->m_device->get_vk_device(), &create_info, nullptr, &swapchain); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to create swapchain: {}", VkResult_string(res)));
			}

			return std::make_unique<T_IMPL>(previous->m_device, previous->m_surface, swapchain, create_info);
		}
	};
} // namespace vvv
