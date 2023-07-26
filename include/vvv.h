#pragma once

#include "details/vvv_error_handling.h"
#include "details/vvv_raw.h"

namespace vvv
{
	template<typename T>
	struct vk_factory_assist
	{
		using base_type	  = typename T::base_type;
		using create_info = typename T::create_info;

		template<typename T_PARENT, typename T_FN, typename... T_ARGS>
		result<std::unique_ptr<T>> operator()(T_PARENT parent, T_FN fn, T_ARGS&&... args)
		{
			auto nfo = std::make_unique<create_info>(parent, std::forward<T_ARGS>(args)...);
			vvv_LEAF_CHECK(fn(nfo));
			return base_type::factory<T>(std::move(nfo));
		}

		template<typename T_PARENT, typename... T_ARGS>
		result<std::unique_ptr<T>> operator()(T_PARENT parent, T_ARGS&&... args)
		{
			return base_type::factory<T>(std::make_unique<create_info>(parent, std::forward<T_ARGS>(args)...));
		}
	};
} // namespace vvv

namespace vvv
{
	class vk_swapchain_create_info : public raw_VkSwapchainCreateInfo
	{
	public:
		using raw_VkSwapchainCreateInfo::raw_VkSwapchainCreateInfo;
		virtual ~vk_swapchain_create_info() = default;
	};

	class vk_swapchain
		: public raw_VkSwapchain
		, public std::enable_shared_from_this<vk_swapchain>
	{
	public:
		using base_type	  = raw_VkSwapchain;
		using create_info = vk_swapchain_create_info;

		using raw_VkSwapchain::raw_VkSwapchain;
		virtual ~vk_swapchain() = default;
	};

} // namespace vvv

namespace vvv
{
	class vk_device_create_info : public raw_VkDeviceCreateInfo
	{
	public:
		using raw_VkDeviceCreateInfo::raw_VkDeviceCreateInfo;
		virtual ~vk_device_create_info() = default;

		auto& with_default_extensions()
		{
			auto enabled_extensions = m_physical_device->filter_extensions({
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
			});

			return with_extensions(std::move(enabled_extensions));
		}

		auto& with_default_queue_families(vk_queue_family_mapping queue_families_mapping)
		{
			std::vector<VkDeviceQueueCreateInfo> queue_families_ci;
			queue_families_ci.reserve(queue_families_mapping.size());

			size_t total_queue_count{0};
			for (size_t i = 0; i < queue_families_mapping.size(); ++i)
			{
				if (queue_families_mapping[i] != vk_queue_family_mapping_invalid)
				{
					bool found{false};
					for (auto& qfci : queue_families_ci)
					{
						if (qfci.queueFamilyIndex == queue_families_mapping[i])
						{
							qfci.queueCount++;
							total_queue_count++;
							found = true;
							break;
						}
					}

					if (!found)
					{
						queue_families_ci.push_back({.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, .queueFamilyIndex = queue_families_mapping[i], .queueCount = 1});
						total_queue_count++;
					}
				}
			}

			std::vector<float> queue_families_ci_prio(total_queue_count);
			for (size_t i = 0, cnt = 0; i < queue_families_ci.size(); ++i)
			{
				queue_families_ci[i].pQueuePriorities = &queue_families_ci_prio[cnt];
				for (size_t j = 0; j < queue_families_ci[i].queueCount; ++j)
				{
					// TODO: need a better way of determining priority, higher == lower prio works for now idk...
					queue_families_ci_prio[cnt + j] = 1.0f / float(j + 1);
				}
				cnt += queue_families_ci[i].queueCount;
			}

			return with_queue_families(std::move(queue_families_ci), std::move(queue_families_ci_prio));
		}
	};

	class vk_device
		: public raw_VkDevice
		, public std::enable_shared_from_this<vk_device>
	{
	public:
		using base_type	  = raw_VkDevice;
		using create_info = vk_device_create_info;

		using raw_VkDevice::raw_VkDevice;
		virtual ~vk_device() = default;

		template<typename T_FN>
		auto create_swapchain(T_FN fn)
		{
			return vk_factory_assist<vk_swapchain>()(shared_from_this(), fn);
		}
	};
} // namespace vvv

namespace vvv
{
	class vk_queue_families_create_info : public raw_VkQueueFamiliesCreateInfo
	{
	public:
		using raw_VkQueueFamiliesCreateInfo::raw_VkQueueFamiliesCreateInfo;
		virtual ~vk_queue_families_create_info() = default;
	};

	class vk_queue_families
		: public raw_VkQueueFamilies
		, public std::enable_shared_from_this<vk_queue_families>
	{
	public:
		using base_type	  = raw_VkQueueFamilies;
		using create_info = vk_queue_families_create_info;

		using raw_VkQueueFamilies::raw_VkQueueFamilies;
		virtual ~vk_queue_families() = default;
	};
} // namespace vvv

namespace vvv
{
	class vk_physical_device_create_info : public raw_VkPhysicalDeviceCreateInfo
	{
	public:
		using raw_VkPhysicalDeviceCreateInfo::raw_VkPhysicalDeviceCreateInfo;

		virtual ~vk_physical_device_create_info() = default;
	};

	class vk_physical_device
		: public raw_VkPhysicalDevice
		, public std::enable_shared_from_this<vk_physical_device>
	{
	public:
		using base_type	  = raw_VkPhysicalDevice;
		using create_info = vk_physical_device_create_info;

		using raw_VkPhysicalDevice::raw_VkPhysicalDevice;
		virtual ~vk_physical_device() = default;

		template<typename T_FN>
		auto create_device(T_FN fn)
		{
			return vk_factory_assist<vk_device>()(shared_from_this(), fn);
		}

		auto create_queue_families()
		{
			return vk_factory_assist<vk_queue_families>()(shared_from_this());
		}
	};
} // namespace vvv

namespace vvv
{
	class vk_surface
		: public raw_VkSurface
		, public std::enable_shared_from_this<vk_surface>
	{
	public:
		using raw_VkSurface::raw_VkSurface;
		virtual ~vk_surface() = default;
	};
} // namespace vvv

namespace vvv
{
	class vk_instance_create_info : public raw_VkInstanceCreateInfo
	{
	public:
		vk_instance_create_info(std::shared_ptr<raw_VkApplication> vk_app) : raw_VkInstanceCreateInfo(vk_app) { }
		virtual ~vk_instance_create_info() = default;
	};

	class vk_instance
		: public raw_VkInstance
		, public std::enable_shared_from_this<vk_instance>
	{
	public:
		using base_type	  = raw_VkInstance;
		using create_info = vk_instance_create_info;

		using raw_VkInstance::raw_VkInstance;
		virtual ~vk_instance() = default;

		result<std::vector<VkPhysicalDeviceCreateInfo>> discover_suitable_physical_devices()
		{
			vvv_LEAF_AUTO(
				physical_devices,
				discover_physical_devices(
					[](const VkPhysicalDeviceCreateInfo& info)
					{
						return info.physicalDeviceFeatures.geometryShader && info.physicalDeviceFeatures.tessellationShader && info.physicalDeviceFeatures.sparseBinding;
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
				[&](const VkPhysicalDeviceCreateInfo& l, const VkPhysicalDeviceCreateInfo& r)
				{
					auto type_score = score_device_type(l.physicalDeviceProperties);
					if (l.physicalDeviceProperties.deviceType == r.physicalDeviceProperties.deviceType)
					{
						if (auto [l_mem, r_mem] = std::tuple{get_device_memory(l.physicalDeviceMemoryProperties), get_device_memory(r.physicalDeviceMemoryProperties)};
							l_mem == r_mem)
						{
							if (auto [l_valid, r_valid] = std::tuple{l.physicalDeviceIDProperties.deviceLUIDValid, r.physicalDeviceIDProperties.deviceLUIDValid};
								l_valid && r_valid)
							{
								const auto* l_uuid = l.physicalDeviceIDProperties.deviceUUID;
								const auto* r_uuid = r.physicalDeviceIDProperties.deviceUUID;
								for (size_t i = 0; i < sizeof(l.physicalDeviceIDProperties.deviceUUID); ++i)
								{
									if (l_uuid[i] != r_uuid[i])
									{
										return l_uuid[i] > r_uuid[i];
									}
								}
							}

							const auto* l_luid = l.physicalDeviceIDProperties.deviceLUID;
							const auto* r_luid = r.physicalDeviceIDProperties.deviceLUID;
							for (size_t i = 0; i < sizeof(l.physicalDeviceIDProperties.deviceLUID); ++i)
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
						return type_score > score_device_type(r.physicalDeviceProperties);
					}
				});

			return physical_devices;
		}

		template<typename T_FN>
		auto create_physical_device(T_FN fn)
		{
			return vk_factory_assist<vk_physical_device>()(shared_from_this(), fn);
		}

#if defined(VK_KHR_win32_surface)
		auto create_surface(HWND hwnd)
		{
			return raw_VkSurface::factory<vk_surface>(shared_from_this(), hwnd);
		}
#endif
	};

} // namespace vvv

namespace vvv
{
	class vk_application_info : public raw_VkApplicationInfo
	{
	public:
		using raw_VkApplicationInfo::raw_VkApplicationInfo;
		virtual ~vk_application_info() = default;
	};

	class vk_application
		: public raw_VkApplication
		, public std::enable_shared_from_this<vk_application>
	{
	public:
		using base_type	  = raw_VkApplication;
		using create_info = vk_application_info;

		using raw_VkApplication::raw_VkApplication;
		virtual ~vk_application() = default;

		template<typename T_FN>
		auto create_instance(T_FN fn)
		{
			return vk_factory_assist<vk_instance>()(shared_from_this(), fn);
		}
	};

	template<typename T_FN>
	static inline result<std::unique_ptr<vk_application>> create_application(T_FN fn)
	{
		auto nfo = std::make_unique<vk_application::create_info>();
		vvv_LEAF_CHECK(fn(nfo));
		return vk_application::base_type::factory<vk_application>(std::move(nfo));
	}
} // namespace vvv
