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

		size_t					m_family_index{exclusive_queue_families::invalid_family};
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
		static inline result<std::unique_ptr<T_IMPL>> factory(std::shared_ptr<raw_Vk> vk)
		{
			VkApplicationInfo application_info =
				{VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr, "Loopunit", VK_MAKE_VERSION(0, 0, 1), "vvv", VK_MAKE_VERSION(0, 0, 1), VK_API_VERSION_1_3};

			VkInstanceCreateInfo instance_create_info = {VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &application_info, 0, nullptr, 0, nullptr};

			// Extensions

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

#if defined(_WIN32)
				VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
				VK_KHR_EXTERNAL_MEMORY_WIN32_EXTENSION_NAME,
				VK_KHR_WIN32_KEYED_MUTEX_EXTENSION_NAME,
				VK_KHR_EXTERNAL_SEMAPHORE_WIN32_EXTENSION_NAME,
				VK_KHR_EXTERNAL_FENCE_WIN32_EXTENSION_NAME,
			// VK_NV_ACQUIRE_WINRT_DISPLAY_EXTENSION_NAME,
#else
				VK_KHR_XCB_SURFACE_EXTENSION_NAME,
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

			if (enabled_layers.size() > 0)
			{
				instance_create_info.enabledLayerCount	 = static_cast<uint32_t>(enabled_layers.size());
				instance_create_info.ppEnabledLayerNames = enabled_layers.data();
			}

			vvv_LEAF_AUTO(
				new_instance,
				[&]() -> result<VkInstance>
				{
					VkInstance new_instance{nullptr};
					if (auto res = vkCreateInstance(&instance_create_info, vk->allocation_callbacks(), &new_instance); res != VK_SUCCESS)
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
		using queue_families = std::array<queue_family_desc, enum_value(queue_family_desc::type::COUNT)>;

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

			assign_queue_family(exclusive_families.gfx_family, queue_family_desc::type::GFX);
			assign_queue_family(exclusive_families.transfer_family, queue_family_desc::type::TRANSFER);
			assign_queue_family(exclusive_families.compute_family, queue_family_desc::type::COMPUTE);
			assign_queue_family(exclusive_families.video_decode_family, queue_family_desc::type::VIDEO_DECODE);
			assign_queue_family(exclusive_families.video_encode_family, queue_family_desc::type::VIDEO_ENCODE);

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

	public:
		raw_VkDevice(std::shared_ptr<raw_VkPhysicalDevice> phys_dev, VkDevice device, VmaAllocator allocator, VolkDeviceTable volk_device_table)
			: m_physical_device{phys_dev}
			, m_device{device}
			, m_allocator{allocator}
			, m_volk_device_table{volk_device_table}
		{ }

		virtual ~raw_VkDevice()
		{
			if (m_device)
			{
				m_volk_device_table.vkDestroyDevice(m_device, m_physical_device->allocation_callbacks());
			}
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
					queue_families_ci_prio.push_back(1.0f);

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

			return std::make_unique<T_IMPL>(phys_dev, device, allocator, vdt);
		}
	};
} // namespace vvv

namespace vvv
{
	class vk_device
		: public raw_VkDevice
		, public std::enable_shared_from_this<vk_device>
	{
	public:
		using raw_VkDevice::raw_VkDevice;
		virtual ~vk_device() = default;
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
	};

	class vk
		: public raw_Vk
		, public std::enable_shared_from_this<vk>
	{
	public:
		using raw_Vk::raw_Vk;
		virtual ~vk() = default;

		auto create_instance()
		{
			return raw_VkInstance::factory<vk_instance>(shared_from_this());
		}

		static inline auto factory(int argc, char** argv)
		{
			return raw_Vk::factory<vk>(argc, argv);
		}
	};
} // namespace vvv