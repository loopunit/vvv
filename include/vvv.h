#pragma once

#include "details/vvv_error_handling.h"

#include <volk.h>

#define VMA_HEAVY_ASSERT(expr)				   assert(expr)
#define VMA_DEDICATED_ALLOCATION			   0
#define VMA_DEBUG_MARGIN					   16
#define VMA_DEBUG_DETECT_CORRUPTION			   1
#define VMA_DEBUG_MIN_BUFFER_IMAGE_GRANULARITY 256
#define VMA_USE_STL_SHARED_MUTEX			   1
#define VMA_MEMORY_BUDGET					   0
#define VMA_STATS_STRING_ENABLED			   0
#define VMA_MAPPING_HYSTERESIS_ENABLED		   0
#define VMA_STATIC_VULKAN_FUNCTIONS			   0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS		   0

#define VMA_VULKAN_VERSION 1003000 // Vulkan 1.3

#define VMA_DEBUG_LOG(format, ...)                                                                                                                                                 \
	do                                                                                                                                                                             \
	{                                                                                                                                                                              \
		printf(format, __VA_ARGS__);                                                                                                                                               \
		printf("\n");                                                                                                                                                              \
	}                                                                                                                                                                              \
	while (false)

#include <vk_mem_alloc.h>

#include <algorithm>
#include <array>
#include <filesystem>

namespace vvv
{
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

	struct exclusive_queue_families
	{
		static inline constexpr size_t invalid_family = 0xffffffffull;
		size_t						   gfx_family{invalid_family};
		size_t						   transfer_family{invalid_family};
		size_t						   compute_family{invalid_family};
		size_t						   video_decode_family{invalid_family};
		size_t						   video_encode_family{invalid_family};
	};

	enum class queue_family_type : uint32_t
	{
		GFX = 0,
		TRANSFER,
		COMPUTE,
		VIDEO_DECODE,
		VIDEO_ENCODE,
		COUNT
	};

	struct queue_family_desc
	{
		size_t					m_family_index{exclusive_queue_families::invalid_family};
		VkQueueFamilyProperties m_properties{};
		queue_family_type		m_type{queue_family_type::COUNT};

		bool m_gfx{false};
		bool m_compute{false};
		bool m_transfer{false};
		bool m_sparse_binding{false};
		bool m_protected{false};
		bool m_video_decode{false};
		bool m_video_encode{false};
		bool m_supports_present{false};

		queue_family_desc() = default;

		queue_family_desc(size_t idx, const VkQueueFamilyProperties& props, queue_family_type t, VkPhysicalDevice phys_dev) : m_family_index{idx}, m_properties{props}, m_type{t}
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
			return m_type != queue_family_type::COUNT;
		}
	};

	class raw_Vk
	{
	protected:
		const VkAllocationCallbacks* m_allocation_callbacks{nullptr};
		VkAllocationCallbacks		 m_custom_allocation_callbacks;

	public:
		raw_Vk()
			: m_custom_allocation_callbacks{
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

		virtual ~raw_Vk() = default;

		auto allocation_callbacks() const
		{
			return m_allocation_callbacks;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(int argc, char** argv)
		{
			::_wputenv_s(L"VK_LAYER_PATH", std::filesystem::path(argv[0]).parent_path().c_str());

			if (auto r = volkInitialize(); r != VK_SUCCESS)
			{
				return vvv_err_not_specified("volkInitialize failed!");
			}

			// auto version = volkGetInstanceVersion();
			// printf("Vulkan version %d.%d.%d initialized.\n", VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));

			return std::make_unique<T_IMPL>();
		}
	};

	class raw_VkApplicationInfo
	{
	protected:
		VkApplicationInfo m_value{};

		std::string m_application_name;
		std::string m_engine_name;

	public:
		raw_VkApplicationInfo()
		{
			m_value.sType	   = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			m_value.apiVersion = VK_API_VERSION_1_3;
		}

		virtual ~raw_VkApplicationInfo() { }

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

	class raw_VkInstanceCreateInfo
	{
	protected:
		VkInstanceCreateInfo m_value{};

		std::shared_ptr<raw_VkApplicationInfo> m_application_info;
		std::vector<const char*>			   m_enabled_layers;
		std::vector<const char*>			   m_enabled_extensions;

	public:
		raw_VkInstanceCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		}

		virtual ~raw_VkInstanceCreateInfo() { }

		auto& with_flags(VkInstanceCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_application_info(std::shared_ptr<raw_VkApplicationInfo> ai)
		{
			m_application_info		 = ai;
			m_value.pApplicationInfo = m_application_info->data();
			return *this;
		}

		auto& with_layers(std::vector<const char*>&& enabled_layers)
		{
			m_enabled_layers			= std::move(enabled_layers);
			m_value.enabledLayerCount	= static_cast<uint32_t>(m_enabled_layers.size());
			m_value.ppEnabledLayerNames = m_enabled_layers.data();
			return *this;
		}

		auto& with_default_layers()
		{
			uint32_t availiable_layer_count = 0;
			vkEnumerateInstanceLayerProperties(&availiable_layer_count, nullptr);
			std::vector<VkLayerProperties> available_layers(availiable_layer_count);
			vkEnumerateInstanceLayerProperties(&availiable_layer_count, available_layers.data());

			std::vector<const char*> enabled_layers;
			const char*				 desired_layers[] = {
				 "VK_LAYER_KHRONOS_validation",
				 "VK_LAYER_NV_optimus",
			 };

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
			uint32_t availiable_extension_count = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &availiable_extension_count, nullptr);
			std::vector<VkExtensionProperties> available_extensions(availiable_extension_count);
			vkEnumerateInstanceExtensionProperties(nullptr, &availiable_extension_count, available_extensions.data());

			const char* desired_extensions[] = {
				VK_KHR_SURFACE_EXTENSION_NAME,
				VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME,
				VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
				VK_KHR_DISPLAY_EXTENSION_NAME,
				VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
				VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
				VK_EXT_DIRECT_MODE_DISPLAY_EXTENSION_NAME,
#if defined(VK_USE_PLATFORM_ANDROID_KHR)
				VK_KHR_ANDROID_SURFACE_EXTENSION_NAME,
#elif defined(VK_USE_PLATFORM_WIN32_KHR)
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
				VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
				VK_KHR_WIN32_KEYED_MUTEX_EXTENSION_NAME,
				VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME,
				VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME,
			// VK_NV_ACQUIRE_WINRT_DISPLAY_EXTENSION_NAME,
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
			};

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

			return with_extensions(std::move(enabled_extensions));
		}

		auto* data()
		{
			return &m_value;
		}
	};

	class raw_VkInstance
	{
	protected:
		std::shared_ptr<raw_Vk>	 m_vk;
		VkInstance				 m_instance{nullptr};
		VkDebugReportCallbackEXT m_debug_report_callbacks{nullptr};

	public:
		raw_VkInstance(std::shared_ptr<raw_Vk> vk, VkInstance inst, VkDebugReportCallbackEXT cb) : m_vk{vk}, m_instance{inst}, m_debug_report_callbacks{cb} { }

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

		auto get_instance() const
		{
			return m_instance;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_Vk> vk, std::shared_ptr<raw_VkInstanceCreateInfo> ci)
		{
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

	class raw_VkPhysicalDevice
	{
	public:
		using queue_families = std::array<queue_family_desc, enum_value(queue_family_type::COUNT)>;

	protected:
		std::shared_ptr<raw_VkInstance>		  m_instance;
		std::shared_ptr<physical_device_info> m_info;

		result<std::vector<VkQueueFamilyProperties>> get_queue_family_properties()
		{
			uint32_t count = 0;
			if (vkGetPhysicalDeviceQueueFamilyProperties(m_info->m_physical_device, &count, nullptr); count > 0)
			{
				std::vector<VkQueueFamilyProperties> queue_family_props(count);

				vkGetPhysicalDeviceQueueFamilyProperties(m_info->m_physical_device, &count, queue_family_props.data());
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
				return exclusive_queue_families::invalid_family;
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

		result<queue_families> build_queue_families()
		{
			queue_families res;
			vvv_LEAF_AUTO(queue_family_props, get_queue_family_properties());
			vvv_LEAF_AUTO(exclusive_families, find_exclusive_queue_families(queue_family_props));

			auto assign_queue_family = [&](auto var, auto flag)
			{
				if (var <= queue_family_props.size())
				{
					res[enum_value(flag)] = queue_family_desc(var, queue_family_props[var], flag, m_info->m_physical_device);
				}
			};

			assign_queue_family(exclusive_families.gfx_family, queue_family_type::GFX);
			assign_queue_family(exclusive_families.transfer_family, queue_family_type::TRANSFER);
			assign_queue_family(exclusive_families.compute_family, queue_family_type::COMPUTE);
			assign_queue_family(exclusive_families.video_decode_family, queue_family_type::VIDEO_DECODE);
			assign_queue_family(exclusive_families.video_encode_family, queue_family_type::VIDEO_ENCODE);

			return res;
		}

		queue_families m_queue_families;

	public:
		raw_VkPhysicalDevice(std::shared_ptr<raw_VkInstance> instance, std::shared_ptr<physical_device_info> info) : m_instance{instance}, m_info{info}
		{
			if (auto queue_families_res = build_queue_families())
			{
				m_queue_families = std::move(*queue_families_res);
			}
			else
			{
				throw queue_families_res.error();
			}
		}

		virtual ~raw_VkPhysicalDevice() = default;

		auto allocation_callbacks() const
		{
			return m_instance->allocation_callbacks();
		}

		auto get_instance() const
		{
			return m_instance->get_instance();
		}

		const queue_families& get_queue_families()
		{
			return m_queue_families;
		}

		const physical_device_info& get_physical_device_info()
		{
			return *m_info;
		}

		auto get_physical_device() const
		{
			return m_info->m_physical_device;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_VkInstance> instance, std::shared_ptr<physical_device_info> info)
		{
			return std::make_unique<T_IMPL>(instance, info);
		}
	};

	class raw_VkDevice
	{
	protected:
		std::shared_ptr<raw_VkPhysicalDevice> m_physical_device;
		VkDevice							  m_device;
		VmaAllocator						  m_allocator;
		VolkDeviceTable						  m_volk_device_table;

		using family_queues = std::array<VkQueue, enum_value(queue_family_type::COUNT)>;
		family_queues m_family_queues{};

	public:
		const VolkDeviceTable& device_table() const
		{
			return m_volk_device_table;
		}

		raw_VkDevice(std::shared_ptr<raw_VkPhysicalDevice> phys_dev, VkDevice device, VmaAllocator allocator, VolkDeviceTable volk_device_table, family_queues device_family_queues)
			: m_physical_device{phys_dev}
			, m_device{device}
			, m_allocator{allocator}
			, m_volk_device_table{volk_device_table}
			, m_family_queues{device_family_queues}
		{ }

		virtual ~raw_VkDevice()
		{
			if (m_device)
			{
				m_volk_device_table.vkDestroyDevice(m_device, m_physical_device->allocation_callbacks());
			}
		}

		auto allocation_callbacks() const
		{
			return m_physical_device->allocation_callbacks();
		}

		auto get_device() const
		{
			return m_device;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_VkPhysicalDevice> phys_dev)
		{
			const auto&							 queue_families = phys_dev->get_queue_families();
			std::vector<VkDeviceQueueCreateInfo> queue_families_ci;
			std::vector<float>					 queue_families_ci_prio;

			queue_families_ci.reserve(queue_families.size());
			queue_families_ci_prio.reserve(queue_families.size());

			for (size_t i = 0; i < queue_families.size(); ++i)
			{
				if (queue_families[i].m_family_index != exclusive_queue_families::invalid_family)
				{
					queue_families_ci_prio.push_back(1.0f); // TODO: determine priorities

					queue_families_ci.push_back(
						{.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, .queueFamilyIndex = static_cast<uint32_t>(queue_families[i].m_family_index), .queueCount = 1});

					queue_families_ci[i].pQueuePriorities = &(*queue_families_ci_prio.rbegin());
				}
			}

			VkPhysicalDeviceFeatures device_features{};

			VkDeviceCreateInfo create_info{};
			create_info.sType				 = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			create_info.pQueueCreateInfos	 = queue_families_ci.data();
			create_info.queueCreateInfoCount = queue_families_ci.size();
			create_info.pEnabledFeatures	 = &device_features;

			const auto& phys_dev_info = phys_dev->get_physical_device_info();

			uint32_t extension_count = 0;
			vkEnumerateDeviceExtensionProperties(phys_dev_info.m_physical_device, nullptr, &extension_count, nullptr);

			std::vector<VkExtensionProperties> available_extensions(extension_count);
			vkEnumerateDeviceExtensionProperties(phys_dev_info.m_physical_device, nullptr, &extension_count, available_extensions.data());

			const char* desired_extensions[] = {
				VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
				VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
				VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME,
				VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
				VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME,
				VK_KHR_FRAGMENT_SHADER_BARYCENTRIC_EXTENSION_NAME,
				VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME,
				VK_KHR_GLOBAL_PRIORITY_EXTENSION_NAME,
				VK_KHR_PIPELINE_EXECUTABLE_PROPERTIES_EXTENSION_NAME,
				VK_KHR_PIPELINE_LIBRARY_EXTENSION_NAME,
				VK_KHR_PRESENT_ID_EXTENSION_NAME,
				VK_KHR_PRESENT_WAIT_EXTENSION_NAME,
				VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
				VK_KHR_RAY_QUERY_EXTENSION_NAME,
				VK_KHR_RAY_TRACING_MAINTENANCE_1_EXTENSION_NAME,
				VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME,
				VK_KHR_SHADER_CLOCK_EXTENSION_NAME,
				VK_KHR_SHADER_SUBGROUP_UNIFORM_CONTROL_FLOW_EXTENSION_NAME,
				VK_KHR_SWAPCHAIN_EXTENSION_NAME,
				VK_KHR_SWAPCHAIN_MUTABLE_FORMAT_EXTENSION_NAME,
				VK_KHR_VIDEO_DECODE_H264_EXTENSION_NAME,
				VK_KHR_VIDEO_DECODE_H265_EXTENSION_NAME,
				VK_KHR_VIDEO_DECODE_QUEUE_EXTENSION_NAME,
				VK_KHR_VIDEO_QUEUE_EXTENSION_NAME,
				VK_KHR_WIN32_KEYED_MUTEX_EXTENSION_NAME,
				VK_KHR_WORKGROUP_MEMORY_EXPLICIT_LAYOUT_EXTENSION_NAME,
				VK_EXT_ATTACHMENT_FEEDBACK_LOOP_LAYOUT_EXTENSION_NAME,
				VK_EXT_BLEND_OPERATION_ADVANCED_EXTENSION_NAME,
				VK_EXT_BORDER_COLOR_SWIZZLE_EXTENSION_NAME,
				VK_EXT_CALIBRATED_TIMESTAMPS_EXTENSION_NAME,
				VK_EXT_COLOR_WRITE_ENABLE_EXTENSION_NAME,
				VK_EXT_CONDITIONAL_RENDERING_EXTENSION_NAME,
				VK_EXT_CONSERVATIVE_RASTERIZATION_EXTENSION_NAME,
				VK_EXT_CUSTOM_BORDER_COLOR_EXTENSION_NAME,
				VK_EXT_DEPTH_CLAMP_ZERO_ONE_EXTENSION_NAME,
				VK_EXT_DEPTH_CLIP_CONTROL_EXTENSION_NAME,
				VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME,
				VK_EXT_DEPTH_RANGE_UNRESTRICTED_EXTENSION_NAME,
				VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME,
				VK_EXT_DISCARD_RECTANGLES_EXTENSION_NAME,
				VK_EXT_EXTENDED_DYNAMIC_STATE_3_EXTENSION_NAME,
				VK_EXT_EXTERNAL_MEMORY_HOST_EXTENSION_NAME,
				VK_EXT_FRAGMENT_SHADER_INTERLOCK_EXTENSION_NAME,
				VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME,
				VK_EXT_GLOBAL_PRIORITY_QUERY_EXTENSION_NAME,
				VK_EXT_GLOBAL_PRIORITY_EXTENSION_NAME,
				VK_EXT_GRAPHICS_PIPELINE_LIBRARY_EXTENSION_NAME,
				VK_EXT_HDR_METADATA_EXTENSION_NAME,
				VK_EXT_IMAGE_2D_VIEW_OF_3D_EXTENSION_NAME,
				VK_EXT_IMAGE_SLICED_VIEW_OF_3D_EXTENSION_NAME,
				VK_EXT_IMAGE_VIEW_MIN_LOD_EXTENSION_NAME,
				VK_EXT_INDEX_TYPE_UINT8_EXTENSION_NAME,
				VK_EXT_LINE_RASTERIZATION_EXTENSION_NAME,
				VK_EXT_LOAD_STORE_OP_NONE_EXTENSION_NAME,
				VK_EXT_MEMORY_BUDGET_EXTENSION_NAME,
				VK_EXT_MEMORY_PRIORITY_EXTENSION_NAME,
				VK_EXT_MESH_SHADER_EXTENSION_NAME,
				VK_EXT_MULTI_DRAW_EXTENSION_NAME,
				VK_EXT_MUTABLE_DESCRIPTOR_TYPE_EXTENSION_NAME,
				VK_EXT_NON_SEAMLESS_CUBE_MAP_EXTENSION_NAME,
				VK_EXT_OPACITY_MICROMAP_EXTENSION_NAME,
				VK_EXT_PAGEABLE_DEVICE_LOCAL_MEMORY_EXTENSION_NAME,
				VK_EXT_PCI_BUS_INFO_EXTENSION_NAME,
				VK_EXT_PIPELINE_LIBRARY_GROUP_HANDLES_EXTENSION_NAME,
				VK_EXT_PIPELINE_ROBUSTNESS_EXTENSION_NAME,
				VK_EXT_POST_DEPTH_COVERAGE_EXTENSION_NAME,
				VK_EXT_PRIMITIVE_TOPOLOGY_LIST_RESTART_EXTENSION_NAME,
				VK_EXT_PRIMITIVES_GENERATED_QUERY_EXTENSION_NAME,
				VK_EXT_PROVOKING_VERTEX_EXTENSION_NAME,
				VK_EXT_QUEUE_FAMILY_FOREIGN_EXTENSION_NAME,
				VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
				VK_EXT_SAMPLE_LOCATIONS_EXTENSION_NAME,
				VK_EXT_SHADER_ATOMIC_FLOAT_EXTENSION_NAME,
				VK_EXT_SHADER_IMAGE_ATOMIC_INT64_EXTENSION_NAME,
				VK_EXT_SHADER_MODULE_IDENTIFIER_EXTENSION_NAME,
				VK_EXT_SHADER_OBJECT_EXTENSION_NAME,
				VK_EXT_TRANSFORM_FEEDBACK_EXTENSION_NAME,
				VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME,
				VK_EXT_VERTEX_INPUT_DYNAMIC_STATE_EXTENSION_NAME,
				VK_EXT_YCBCR_IMAGE_ARRAYS_EXTENSION_NAME,
				VK_NV_ACQUIRE_WINRT_DISPLAY_EXTENSION_NAME,
				VK_NV_CLIP_SPACE_W_SCALING_EXTENSION_NAME,
				VK_NV_COMPUTE_SHADER_DERIVATIVES_EXTENSION_NAME,
				VK_NV_COOPERATIVE_MATRIX_EXTENSION_NAME,
				VK_NV_COPY_MEMORY_INDIRECT_EXTENSION_NAME,
				VK_NV_CORNER_SAMPLED_IMAGE_EXTENSION_NAME,
				VK_NV_COVERAGE_REDUCTION_MODE_EXTENSION_NAME,
				//"VK_NV_cuda_kernel_launch", // TODO: this one is undocumented and interesting!
				VK_NV_DEDICATED_ALLOCATION_IMAGE_ALIASING_EXTENSION_NAME,
				VK_NV_DEVICE_DIAGNOSTIC_CHECKPOINTS_EXTENSION_NAME,
				VK_NV_DEVICE_DIAGNOSTICS_CONFIG_EXTENSION_NAME,
				VK_NV_DEVICE_GENERATED_COMMANDS_EXTENSION_NAME,
				VK_NV_FILL_RECTANGLE_EXTENSION_NAME,
				VK_NV_FRAGMENT_COVERAGE_TO_COLOR_EXTENSION_NAME,
				VK_NV_FRAGMENT_SHADING_RATE_ENUMS_EXTENSION_NAME,
				VK_NV_FRAMEBUFFER_MIXED_SAMPLES_EXTENSION_NAME,
				VK_NV_GEOMETRY_SHADER_PASSTHROUGH_EXTENSION_NAME,
				VK_NV_INHERITED_VIEWPORT_SCISSOR_EXTENSION_NAME,
				VK_NV_LINEAR_COLOR_ATTACHMENT_EXTENSION_NAME,
				VK_NV_LOW_LATENCY_EXTENSION_NAME,
				VK_NV_MEMORY_DECOMPRESSION_EXTENSION_NAME,
				VK_NV_MESH_SHADER_EXTENSION_NAME,
				VK_NV_OPTICAL_FLOW_EXTENSION_NAME,
				VK_NV_RAY_TRACING_EXTENSION_NAME,
				VK_NV_RAY_TRACING_INVOCATION_REORDER_EXTENSION_NAME,
				VK_NV_RAY_TRACING_MOTION_BLUR_EXTENSION_NAME,
				VK_NV_REPRESENTATIVE_FRAGMENT_TEST_EXTENSION_NAME,
				VK_NV_SAMPLE_MASK_OVERRIDE_COVERAGE_EXTENSION_NAME,
				VK_NV_SCISSOR_EXCLUSIVE_EXTENSION_NAME,
				VK_NV_SHADER_IMAGE_FOOTPRINT_EXTENSION_NAME,
				VK_NV_SHADER_SM_BUILTINS_EXTENSION_NAME,
				VK_NV_SHADER_SUBGROUP_PARTITIONED_EXTENSION_NAME,
				VK_NV_SHADING_RATE_IMAGE_EXTENSION_NAME,
				VK_NV_VIEWPORT_ARRAY2_EXTENSION_NAME,
				VK_NV_VIEWPORT_SWIZZLE_EXTENSION_NAME,
				VK_KHR_WIN32_KEYED_MUTEX_EXTENSION_NAME,
				VK_NVX_BINARY_IMPORT_EXTENSION_NAME,
				VK_NVX_IMAGE_VIEW_HANDLE_EXTENSION_NAME,
				VK_NVX_MULTIVIEW_PER_VIEW_ATTRIBUTES_EXTENSION_NAME,
				VK_AMD_BUFFER_MARKER_EXTENSION_NAME,
				VK_KHR_GET_MEMORY_REQUIREMENTS_2_EXTENSION_NAME,
				VK_KHR_BIND_MEMORY_2_EXTENSION_NAME,
				VK_KHR_EXTERNAL_MEMORY_CAPABILITIES_EXTENSION_NAME,
				VK_KHR_EXTERNAL_MEMORY_EXTENSION_NAME,
				VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
			};

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

			if (enabled_extensions.size() > 0)
			{
				create_info.enabledExtensionCount	= static_cast<uint32_t>(enabled_extensions.size());
				create_info.ppEnabledExtensionNames = enabled_extensions.data();
			}

			// Device layers are deprecated and ignored
			create_info.enabledLayerCount	= 0;
			create_info.ppEnabledLayerNames = nullptr;

			VkDevice device{nullptr};
			if (auto res = vkCreateDevice(phys_dev_info.m_physical_device, &create_info, phys_dev->allocation_callbacks(), &device); res != VK_SUCCESS) [[unlikely]]
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
			allocatorCreateInfo.physicalDevice		   = phys_dev_info.m_physical_device;
			allocatorCreateInfo.device				   = device;
			allocatorCreateInfo.instance			   = phys_dev->get_instance();
			allocatorCreateInfo.pVulkanFunctions	   = &vma_funcs;

			VmaAllocator allocator;
			if (auto res = vmaCreateAllocator(&allocatorCreateInfo, &allocator); res != VK_SUCCESS) [[unlikely]]
			{
				vdt.vkDestroyDevice(device, phys_dev->allocation_callbacks());
				return vvv_err_not_specified(fmt::format("vmaCreateAllocatorfailed with: {}", VkResult_string(res)));
			}

			std::array<VkQueue, enum_value(queue_family_type::COUNT)> device_family_queues{};

			auto get_family_queue = [&](queue_family_type q_type) -> result<void>
			{
				auto& queue_famlies_desc = phys_dev->get_queue_families();
				auto  family_index		 = queue_famlies_desc[enum_value(q_type)].m_family_index;
				if (family_index != exclusive_queue_families::invalid_family)
				{
					vdt.vkGetDeviceQueue(device, family_index, 0, &device_family_queues[enum_value(q_type)]);
					if (!device_family_queues[enum_value(q_type)])
					{
						return vvv_err_not_specified(fmt::format("vkGetDeviceQueue failed for family index: {}", family_index));
					}
					return {};
				}
				return vvv_err_not_specified(fmt::format("vkGetDeviceQueue was not valid for family index: {}", family_index));
			};

			vvv_CHECK(get_family_queue(queue_family_type::GFX));
			vvv_CHECK(get_family_queue(queue_family_type::COMPUTE));
			vvv_CHECK(get_family_queue(queue_family_type::TRANSFER));
			vvv_CHECK(get_family_queue(queue_family_type::VIDEO_DECODE));

			return std::make_unique<T_IMPL>(phys_dev, device, allocator, vdt, device_family_queues);
		}
	};

	struct surface_capabilities
	{
		VkSurfaceCapabilitiesKHR		m_capabilities;
		std::vector<VkSurfaceFormatKHR> m_formats;
		std::vector<VkPresentModeKHR>	m_present_modes;
	};

	class raw_VkSurface
	{
	protected:
		std::shared_ptr<raw_VkInstance> m_instance;
		VkSurfaceKHR					m_surface{nullptr};

	public:
		raw_VkSurface(std::shared_ptr<raw_VkInstance> instance, VkSurfaceKHR surface) : m_instance{instance}, m_surface{surface} { }

		virtual ~raw_VkSurface()
		{
			if (m_instance)
			{
				vkDestroySurfaceKHR(m_instance->get_instance(), m_surface, m_instance->allocation_callbacks());
			}
		}

		auto get_surface() const
		{
			return m_surface;
		}

		result<surface_capabilities> get_surface_capabilities(raw_VkPhysicalDevice& physical_dev)
		{
			surface_capabilities cap;
			if (auto res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_dev.get_physical_device(), m_surface, &cap.m_capabilities); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface caps: {}", VkResult_string(res)));
			}

			uint32_t format_count{0};
			if (auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_dev.get_physical_device(), m_surface, &format_count, nullptr); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface formats: {}", VkResult_string(res)));
			}

			cap.m_formats.resize(format_count);
			if (auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(physical_dev.get_physical_device(), m_surface, &format_count, cap.m_formats.data()); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface formats: {}", VkResult_string(res)));
			}

			uint32_t present_mode_count{0};
			if (auto res = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_dev.get_physical_device(), m_surface, &present_mode_count, nullptr); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface present modes: {}", VkResult_string(res)));
			}

			cap.m_present_modes.resize(present_mode_count);
			if (auto res = vkGetPhysicalDeviceSurfacePresentModesKHR(physical_dev.get_physical_device(), m_surface, &present_mode_count, cap.m_present_modes.data());
				res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to get surface present modes: {}", VkResult_string(res)));
			}

			return cap;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_VkInstance> instance, HWND hwnd)
		{
			VkWin32SurfaceCreateInfoKHR create_info{};
			create_info.sType	  = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			create_info.hwnd	  = hwnd;
			create_info.hinstance = ::GetModuleHandleA(nullptr);

			VkSurfaceKHR surface{nullptr};
			if (auto res = vkCreateWin32SurfaceKHR(instance->get_instance(), &create_info, instance->allocation_callbacks(), &surface); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to create surface: {}", VkResult_string(res)));
			}

			return std::make_unique<T_IMPL>(instance, surface);
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
			if (auto res = m_device->device_table().vkGetSwapchainImagesKHR(m_device->get_device(), m_swapchain, &image_count, nullptr); res != VK_SUCCESS)
			{
				vvv_throw_not_specified(fmt::format("Unable get swapchain images {}", VkResult_string(res)));
			}

			m_swapchain_images.resize(image_count);

			if (auto res = m_device->device_table().vkGetSwapchainImagesKHR(m_device->get_device(), m_swapchain, &image_count, m_swapchain_images.data()); res != VK_SUCCESS)
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

				if (auto res = m_device->device_table().vkCreateImageView(m_device->get_device(), &create_info, m_device->allocation_callbacks(), &m_swapchain_image_views[i]);
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
					m_device->device_table().vkDestroyImageView(m_device->get_device(), img_view, m_device->allocation_callbacks());
				}
			}

			if (m_swapchain)
			{
				m_device->device_table().vkDestroySwapchainKHR(m_device->get_device(), m_swapchain, m_device->allocation_callbacks());
			}
		}

		auto get_surface_format() const
		{
			return m_create_info.imageFormat;
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(
			std::shared_ptr<raw_VkDevice>  device,
			std::shared_ptr<raw_VkSurface> surface,
			VkExtent2D					   swapchain_size,
			uint32_t					   image_count,
			VkFormat					   surface_format,
			VkColorSpaceKHR				   color_space,
			VkSurfaceTransformFlagBitsKHR  transform,
			VkPresentModeKHR			   present_mode,
			VkCompositeAlphaFlagBitsKHR	   composite)
		{
			VkSwapchainCreateInfoKHR create_info{};
			create_info.sType				  = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			create_info.surface				  = surface->get_surface();
			create_info.minImageCount		  = image_count;
			create_info.imageFormat			  = surface_format;
			create_info.imageColorSpace		  = color_space;
			create_info.imageExtent			  = swapchain_size;
			create_info.imageArrayLayers	  = 1;
			create_info.imageUsage			  = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
			create_info.imageSharingMode	  = VK_SHARING_MODE_EXCLUSIVE;
			create_info.queueFamilyIndexCount = 0;
			create_info.pQueueFamilyIndices	  = nullptr;
			create_info.preTransform		  = transform;
			create_info.compositeAlpha		  = composite;
			create_info.presentMode			  = present_mode;
			create_info.clipped				  = VK_TRUE;
			create_info.oldSwapchain		  = VK_NULL_HANDLE;

			VkSwapchainKHR swapchain{nullptr};
			if (auto res = device->device_table().vkCreateSwapchainKHR(device->get_device(), &create_info, nullptr, &swapchain); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to create swapchain: {}", VkResult_string(res)));
			}

			return std::make_unique<T_IMPL>(device, surface, swapchain, create_info);
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_VkSwapchain> previous, VkExtent2D swapchain_size)
		{
			VkSwapchainCreateInfoKHR create_info{previous->m_create_info};
			create_info.imageExtent	 = swapchain_size;
			create_info.oldSwapchain = previous->m_swapchain;

			VkSwapchainKHR swapchain{nullptr};
			if (auto res = previous->m_device->device_table().vkCreateSwapchainKHR(previous->m_device->get_device(), &create_info, nullptr, &swapchain); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Unable to create swapchain: {}", VkResult_string(res)));
			}

			return std::make_unique<T_IMPL>(previous->m_device, previous->m_surface, swapchain, create_info);
		}
	};

	//

	class raw_VkPipelineShaderStageCreateInfo
	{
	protected:
		VkPipelineShaderStageCreateInfo m_value{};
		std::string						m_name;

	public:
		raw_VkPipelineShaderStageCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineShaderStageCreateInfo() { }

		auto& with_stage(VkPipelineShaderStageCreateFlags flags, VkShaderStageFlagBits stage)
		{
			m_value.flags = flags;
			m_value.stage = stage;
			return *this;
		}

		auto& with_module(VkShaderModule module)
		{
			m_value.module = module;
			return *this;
		}

		auto& with_name(std::string&& name)
		{
			m_name		  = std::move(name);
			m_value.pName = m_name.c_str();
			return *this;
		}
	};

	class raw_VkPipelineVertexInputStateCreateInfo
	{
	protected:
		VkPipelineVertexInputStateCreateInfo		   m_value{};
		std::vector<VkVertexInputBindingDescription>   m_vertex_binding_descriptions;
		std::vector<VkVertexInputAttributeDescription> m_vertex_attribute_descriptions;

	public:
		raw_VkPipelineVertexInputStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineVertexInputStateCreateInfo() { }

		auto& with_flags(VkPipelineVertexInputStateCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_binding_descriptions(std::vector<VkVertexInputBindingDescription>&& vertex_descriptions)
		{
			m_vertex_binding_descriptions		  = std::move(vertex_descriptions);
			m_value.vertexBindingDescriptionCount = static_cast<uint32_t>(m_vertex_binding_descriptions.size());
			m_value.pVertexBindingDescriptions	  = m_vertex_binding_descriptions.data();
			return *this;
		}

		auto& with_attribute_descriptions(std::vector<VkVertexInputAttributeDescription>&& vertex_descriptions)
		{
			m_vertex_attribute_descriptions			= std::move(vertex_descriptions);
			m_value.vertexAttributeDescriptionCount = static_cast<uint32_t>(m_vertex_attribute_descriptions.size());
			m_value.pVertexAttributeDescriptions	= m_vertex_attribute_descriptions.data();
			return *this;
		}
	};

	class raw_VkPipelineInputAssemblyStateCreateInfo
	{
	protected:
		VkPipelineInputAssemblyStateCreateInfo m_value{};

	public:
		raw_VkPipelineInputAssemblyStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineInputAssemblyStateCreateInfo() { }

		auto& with_flags(VkPipelineInputAssemblyStateCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_topology(VkPrimitiveTopology topology)
		{
			m_value.topology = topology;
			return *this;
		}

		auto& with_primitive_restart_enable(VkBool32 primitiveRestartEnable)
		{
			m_value.primitiveRestartEnable = primitiveRestartEnable;
			return *this;
		}
	};

	class raw_VkPipelineTessellationStateCreateInfo
	{
	protected:
		VkPipelineTessellationStateCreateInfo m_value{};

	public:
		raw_VkPipelineTessellationStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineTessellationStateCreateInfo() { }

		auto& with_flags(VkPipelineTessellationStateCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_patch_control_points(uint32_t patchControlPoints)
		{
			m_value.patchControlPoints = patchControlPoints;
			return *this;
		}
	};

	class raw_VkPipelineViewportStateCreateInfo
	{
	protected:
		VkPipelineViewportStateCreateInfo m_value{};

		VkViewport m_viewport{};
		VkRect2D   m_scissor{};

		std::vector<VkViewport> m_viewports;
		std::vector<VkRect2D>	m_scissors;

	public:
		raw_VkPipelineViewportStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineViewportStateCreateInfo() { }

		auto& with_flags(VkPipelineViewportStateCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_viewport(VkViewport viewport)
		{
			m_viewport			  = viewport;
			m_value.viewportCount = 1;
			m_value.pViewports	  = &m_viewport;
			return *this;
		}

		auto& with_viewports(std::vector<VkViewport>&& viewports)
		{
			m_viewports			  = std::move(viewports);
			m_value.viewportCount = static_cast<uint32_t>(m_viewports.size());
			m_value.pViewports	  = m_viewports.data();
			return *this;
		}

		auto& with_scissor(VkRect2D scissor)
		{
			m_scissor			 = scissor;
			m_value.scissorCount = 1;
			m_value.pScissors	 = &m_scissor;
			return *this;
		}

		auto& with_scissors(std::vector<VkRect2D>&& scissors)
		{
			m_scissors			 = std::move(scissors);
			m_value.scissorCount = static_cast<uint32_t>(m_scissors.size());
			m_value.pScissors	 = m_scissors.data();
			return *this;
		}
	};

	class raw_VkPipelineRasterizationStateCreateInfo
	{
	protected:
		VkPipelineRasterizationStateCreateInfo m_value{};

	public:
		raw_VkPipelineRasterizationStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineRasterizationStateCreateInfo() { }

		auto& with_flags(VkPipelineRasterizationStateCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_depth_clamp_enable(VkBool32 depthClampEnable)
		{
			m_value.depthClampEnable = depthClampEnable;
			return *this;
		}

		auto& with_rasterizer_discard_enable(VkBool32 rasterizerDiscardEnable)
		{
			m_value.rasterizerDiscardEnable = rasterizerDiscardEnable;
			return *this;
		}

		auto& with_polygon_mode(VkPolygonMode polygonMode)
		{
			m_value.polygonMode = polygonMode;
			return *this;
		}

		auto& with_cull_mode(VkCullModeFlags cullMode)
		{
			m_value.cullMode = cullMode;
			return *this;
		}

		auto& with_front_face(VkFrontFace frontFace)
		{
			m_value.frontFace = frontFace;
			return *this;
		}

		auto& with_depth_bias(VkBool32 depthBiasEnable, float depthBiasConstantFactor, float depthBiasClamp, float depthBiasSlopeFactor)
		{
			m_value.depthBiasEnable			= depthBiasEnable;
			m_value.depthBiasConstantFactor = depthBiasConstantFactor;
			m_value.depthBiasClamp			= depthBiasClamp;
			m_value.depthBiasSlopeFactor	= depthBiasSlopeFactor;
			return *this;
		}

		auto& with_line_width(float lineWidth)
		{
			m_value.lineWidth = lineWidth;
			return *this;
		}
	};

	class raw_VkPipelineMultisampleStateCreateInfo
	{
	protected:
		VkPipelineMultisampleStateCreateInfo m_value{};

	public:
		raw_VkPipelineMultisampleStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineMultisampleStateCreateInfo() { }

		auto& with_flags(VkPipelineMultisampleStateCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_rasterization_samples(VkSampleCountFlagBits rasterizationSamples)
		{
			m_value.rasterizationSamples = rasterizationSamples;
			return *this;
		}

		auto& with_sample_shading(VkBool32 sampleShadingEnable, float minSampleShading)
		{
			m_value.sampleShadingEnable = sampleShadingEnable;
			m_value.minSampleShading	= minSampleShading;
			return *this;
		}

		auto& with_sample_mask(const VkSampleMask* pSampleMask)
		{
			m_value.pSampleMask = pSampleMask;
			return *this;
		}

		auto& with_alpha_coverage_enable(VkBool32 alphaToCoverageEnable)
		{
			m_value.alphaToCoverageEnable = alphaToCoverageEnable;
			return *this;
		}

		auto& with_alpha_to_one_enable(VkBool32 alphaToOneEnable)
		{
			m_value.alphaToOneEnable = alphaToOneEnable;
			return *this;
		}
	};

	class raw_VkPipelineDepthStencilStateCreateInfo
	{
	protected:
		VkPipelineDepthStencilStateCreateInfo m_value;

	public:
		raw_VkPipelineDepthStencilStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineDepthStencilStateCreateInfo() { }

		auto& with_flags(VkPipelineDepthStencilStateCreateFlags flags)
		{
			return *this;
		}

		auto&
		with_depth(VkBool32 depthTestEnable, VkBool32 depthWriteEnable, VkCompareOp depthCompareOp, VkBool32 depthBoundsTestEnable, float minDepthBounds, float maxDepthBounds)
		{
			m_value.depthTestEnable		  = depthTestEnable;
			m_value.depthWriteEnable	  = depthWriteEnable;
			m_value.depthCompareOp		  = depthCompareOp;
			m_value.depthBoundsTestEnable = depthBoundsTestEnable;
			m_value.minDepthBounds		  = minDepthBounds;
			m_value.maxDepthBounds		  = maxDepthBounds;

			return *this;
		}

		auto& with_stencil(VkBool32 stencilTestEnable, VkStencilOpState front, VkStencilOpState back)
		{
			m_value.stencilTestEnable = stencilTestEnable;
			m_value.front			  = front;
			m_value.back			  = back;

			return *this;
		}
	};

	class raw_VkPipelineColorBlendStateCreateInfo
	{
	protected:
		VkPipelineColorBlendStateCreateInfo m_value{};

		VkPipelineColorBlendAttachmentState				 m_attachment_state{};
		std::vector<VkPipelineColorBlendAttachmentState> m_attachment_states;

	public:
		raw_VkPipelineColorBlendStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineColorBlendStateCreateInfo() { }

		auto& with_flags(VkPipelineColorBlendStateCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_logic_op(VkBool32 logicOpEnable, VkLogicOp logicOp)
		{
			m_value.logicOpEnable = logicOpEnable;
			m_value.logicOp		  = logicOp;
			return *this;
		}

		auto& with_attachment(VkPipelineColorBlendAttachmentState attachment)
		{
			m_attachment_state		= attachment;
			m_value.attachmentCount = 1;
			m_value.pAttachments	= &m_attachment_state;
			return *this;
		}

		auto& with_attachments(std::vector<VkPipelineColorBlendAttachmentState>&& attachments)
		{
			m_attachment_states		= std::move(attachments);
			m_value.attachmentCount = static_cast<uint32_t>(m_attachment_states.size());
			m_value.pAttachments	= m_attachment_states.data();
			return *this;
		}

		auto& with_blend_constants(float blendConstants[4])
		{
			for (int i = 0; i < sizeof(blendConstants) / sizeof(blendConstants[0]); ++i)
			{
				m_value.blendConstants[i] = blendConstants[i];
			}
			return *this;
		}
	};

	class raw_VkPipelineDynamicStateCreateInfo
	{
	protected:
		VkPipelineDynamicStateCreateInfo m_value{};

		VkDynamicState				m_dynamic_state;
		std::vector<VkDynamicState> m_dynamic_states;

	public:
		raw_VkPipelineDynamicStateCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		}

		virtual ~raw_VkPipelineDynamicStateCreateInfo() { }

		auto& with_flags(VkPipelineDynamicStateCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_dynamic_state(VkDynamicState ds)
		{
			m_dynamic_state			  = ds;
			m_value.dynamicStateCount = 1;
			m_value.pDynamicStates	  = &m_dynamic_state;
			return *this;
		}

		auto& with_dynamic_states(std::vector<VkDynamicState>&& ds)
		{
			m_dynamic_states		  = std::move(ds);
			m_value.dynamicStateCount = static_cast<uint32_t>(m_dynamic_states.size());
			m_value.pDynamicStates	  = m_dynamic_states.data();
			return *this;
		}
	};

	//

	class raw_VkPipelineLayoutCreateInfo
	{
	protected:
		VkPipelineLayoutCreateInfo m_value;

		VkDescriptorSetLayout			   m_layout;
		std::vector<VkDescriptorSetLayout> m_layouts;

		VkPushConstantRange				 m_constant_range;
		std::vector<VkPushConstantRange> m_constant_ranges;

	public:
		raw_VkPipelineLayoutCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		}

		virtual ~raw_VkPipelineLayoutCreateInfo() { }

		auto& with_flags(VkPipelineLayoutCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_layout(VkDescriptorSetLayout layout)
		{
			m_layout			   = layout;
			m_value.setLayoutCount = 1;
			m_value.pSetLayouts	   = &m_layout;
			return *this;
		}

		auto& with_layouts(std::vector<VkDescriptorSetLayout>&& layouts)
		{
			m_layouts			   = std::move(layouts);
			m_value.setLayoutCount = static_cast<uint32_t>(m_layouts.size());
			m_value.pSetLayouts	   = m_layouts.data();
			return *this;
		}

		auto& with_constant_range(VkPushConstantRange range)
		{
			m_constant_range			   = range;
			m_value.pushConstantRangeCount = 1;
			m_value.pPushConstantRanges	   = &m_constant_range;
			return *this;
		}

		auto& with_constant_ranges(std::vector<VkPushConstantRange>&& ranges)
		{
			m_constant_ranges			   = std::move(ranges);
			m_value.pushConstantRangeCount = static_cast<uint32_t>(m_constant_ranges.size());
			m_value.pPushConstantRanges	   = m_constant_ranges.data();
			return *this;
		}
	};

	class raw_VkPipelineLayout
	{
	protected:
		std::shared_ptr<raw_VkDevice> m_device;
		VkPipelineLayout			  m_pipeline_layout{nullptr};

	public:
		raw_VkPipelineLayout(std::shared_ptr<raw_VkDevice> device, VkPipelineLayout pipeline_layout) : m_device{device}, m_pipeline_layout{pipeline_layout} { }

		virtual ~raw_VkPipelineLayout()
		{
			if (m_pipeline_layout)
			{
				m_device->device_table().vkDestroyPipelineLayout(m_device->get_device(), m_pipeline_layout, m_device->allocation_callbacks());
			}
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_VkDevice> device, std::shared_ptr<raw_VkPipelineLayoutCreateInfo> create_info)
		{
			VkPipelineLayout m_pipeline_layout{nullptr};
			if (auto res = device->device_table().vkCreatePipelineLayout(device->get_device(), create_info.get(), device->allocation_callbacks(), &m_pipeline_layout);
				res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Failed to create pipeline layout: {}", VkResult_string(res)));
			}
			return std::make_unique<T_IMPL>(device);
		}
	};

	//

	class raw_VkRenderPassCreateInfo
	{
	protected:
		VkRenderPassCreateInfo m_value;

		VkAttachmentDescription				 m_attachment;
		std::vector<VkAttachmentDescription> m_attachments;

		VkSubpassDescription			  m_subpass;
		std::vector<VkSubpassDescription> m_subpasses;

		VkSubpassDependency				 m_dependency;
		std::vector<VkSubpassDependency> m_dependencies;

	public:
		raw_VkRenderPassCreateInfo()
		{
			m_value.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		}

		virtual ~raw_VkRenderPassCreateInfo() { }

		auto& with_flags(VkRenderPassCreateFlags flags)
		{
			m_value.flags = flags;
			return *this;
		}

		auto& with_attachment(VkAttachmentDescription att)
		{
			m_attachment			= att;
			m_value.attachmentCount = 1;
			m_value.pAttachments	= &m_attachment;
			return *this;
		}

		auto& with_attachments(std::vector<VkAttachmentDescription>&& att)
		{
			m_attachments			= std::move(att);
			m_value.attachmentCount = static_cast<uint32_t>(m_attachments.size());
			m_value.pAttachments	= m_attachments.data();
			return *this;
		}

		auto& with_subpass(VkSubpassDescription sub)
		{
			m_subpass			 = sub;
			m_value.subpassCount = 1;
			m_value.pSubpasses	 = &m_subpass;
			return *this;
		}

		auto& with_subpasses(std::vector<VkSubpassDescription>&& sub)
		{
			m_subpasses			 = std::move(sub);
			m_value.subpassCount = static_cast<uint32_t>(m_subpasses.size());
			m_value.pSubpasses	 = m_subpasses.data();
			return *this;
		}

		auto& with_dependency(VkSubpassDependency dep)
		{
			m_dependency			= dep;
			m_value.dependencyCount = 1;
			m_value.pDependencies	= &m_dependency;
			return *this;
		}

		auto& with_subpasses(std::vector<VkSubpassDependency>&& dep)
		{
			m_dependencies			= std::move(dep);
			m_value.dependencyCount = static_cast<uint32_t>(m_dependencies.size());
			m_value.pDependencies	= m_dependencies.data();
			return *this;
		}
	};

	class raw_VkRenderPass
	{
	protected:
		std::shared_ptr<raw_VkDevice>	 m_device;
		std::shared_ptr<raw_VkSwapchain> m_swapchain;
		VkRenderPass					 m_render_pass{nullptr};

	public:
		raw_VkRenderPass(std::shared_ptr<raw_VkDevice> device, VkRenderPass render_pass) : m_device{device}, m_render_pass{render_pass} { }

		virtual ~raw_VkRenderPass()
		{
			if (m_render_pass)
			{
				m_device->device_table().vkDestroyRenderPass(m_device->get_device(), m_render_pass, m_device->allocation_callbacks());
			}
		}

		template<typename T_IMPL>
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_VkDevice> device, std::shared_ptr<raw_VkRenderPassCreateInfo> create_info)
		{
			VkRenderPass render_pass{nullptr};
			if (auto res = device->device_table().vkCreateRenderPass(device->get_device(), create_info.get(), device->allocation_callbacks(), &render_pass); res != VK_SUCCESS)
			{
				return vvv_err_not_specified(fmt::format("Failed to create render pass: {}", VkResult_string(res)));
			}

			return std::make_unique<T_IMPL>(device, render_pass);
		}
	};
} // namespace vvv

namespace vvv
{
	class vk_swapchain
		: public raw_VkSwapchain
		, public std::enable_shared_from_this<vk_swapchain>
	{
	public:
		using raw_VkSwapchain::raw_VkSwapchain;
		virtual ~vk_swapchain() = default;

		result<std::unique_ptr<vk_swapchain>> recreate(VkExtent2D swapchain_size) { }
	};

	class vk_surface
		: public raw_VkSurface
		, public std::enable_shared_from_this<vk_surface>
	{
	public:
		using raw_VkSurface::raw_VkSurface;
		virtual ~vk_surface() = default;
	};

	class vk_device
		: public raw_VkDevice
		, public std::enable_shared_from_this<vk_device>
	{
	public:
		using raw_VkDevice::raw_VkDevice;
		virtual ~vk_device() = default;

		result<std::unique_ptr<vk_swapchain>> create_swapchain(
			std::shared_ptr<raw_VkSurface> surface,
			VkExtent2D					   swapchain_size,
			uint32_t					   image_count,
			VkFormat					   surface_format,
			VkColorSpaceKHR				   color_space,
			VkSurfaceTransformFlagBitsKHR  transform,
			VkPresentModeKHR			   present_mode = VK_PRESENT_MODE_MAILBOX_KHR,
			VkCompositeAlphaFlagBitsKHR	   composite	= VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		{
			return raw_VkSwapchain::factory<
				vk_swapchain>(shared_from_this(), surface, swapchain_size, image_count, surface_format, color_space, transform, present_mode, composite);
		}
	};

	class vk_physical_device
		: public raw_VkPhysicalDevice
		, public std::enable_shared_from_this<vk_physical_device>
	{
	public:
		using raw_VkPhysicalDevice::raw_VkPhysicalDevice;
		virtual ~vk_physical_device() = default;

		auto create_device()
		{
			return raw_VkDevice::factory<vk_device>(shared_from_this());
		}
	};

	class vk_instance
		: public raw_VkInstance
		, public std::enable_shared_from_this<vk_instance>
	{
	public:
		using raw_VkInstance::raw_VkInstance;
		virtual ~vk_instance() = default;

		template<typename T_FUN>
		result<std::vector<std::unique_ptr<physical_device_info>>> discover_physical_devices(T_FUN filter_fun)
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

		auto create_physical_device(std::shared_ptr<physical_device_info> dev_info)
		{
			return raw_VkPhysicalDevice::factory<vk_physical_device>(shared_from_this(), dev_info);
		}

		auto create_surface(HWND hwnd)
		{
			return raw_VkSurface::factory<vk_surface>(shared_from_this(), hwnd);
		}
	};

	class vk_application_info
		: public raw_VkApplicationInfo
		, public std::enable_shared_from_this<vk_application_info>
	{
	public:
		using raw_VkApplicationInfo::raw_VkApplicationInfo;
		virtual ~vk_application_info() = default;

		static inline auto factory()
		{
			return std::make_shared<vk_application_info>();
		}
	};

	class vk_instance_create_info
		: public raw_VkInstanceCreateInfo
		, public std::enable_shared_from_this<vk_instance_create_info>
	{
	public:
		vk_instance_create_info(std::shared_ptr<raw_VkApplicationInfo> app_info) : raw_VkInstanceCreateInfo()
		{
			this->with_application_info(app_info);
		}

		virtual ~vk_instance_create_info() = default;

		static inline auto factory(std::shared_ptr<raw_VkApplicationInfo> app_info)
		{
			return std::make_shared<vk_instance_create_info>(app_info);
		}
	};

	class vk
		: public raw_Vk
		, public std::enable_shared_from_this<vk>
	{
	public:
		using raw_Vk::raw_Vk;
		virtual ~vk() = default;

		auto create_instance(std::shared_ptr<raw_VkInstanceCreateInfo> ci)
		{
			return raw_VkInstance::factory<vk_instance>(shared_from_this(), ci);
		}

		static inline auto factory(int argc, char** argv)
		{
			return raw_Vk::factory<vk>(argc, argv);
		}
	};
} // namespace vvv