#pragma once

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
} // namespace vvv

namespace vvv
{
	struct VkPhysicalDeviceCreateInfo
	{
		VkPhysicalDevice				 physicalDevice{nullptr};
		VkPhysicalDeviceProperties		 physicalDeviceProperties{};
		VkPhysicalDeviceFeatures		 physicalDeviceFeatures{};
		VkPhysicalDeviceIDProperties	 physicalDeviceIDProperties{VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES};
		VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties{};
	};

	enum class vk_queue_family_type : uint32_t
	{
		GFX = 0,
		TRANSFER,
		COMPUTE,
		VIDEO_DECODE,
		VIDEO_ENCODE,
		PRESENT, // not a real queue
		COUNT,
	};

	using vk_queue_family_mapping = std::array<uint32_t, enum_value(vk_queue_family_type::COUNT)>;

	constexpr uint32_t vk_queue_family_mapping_invalid{0u - 1u};

	constexpr vk_queue_family_mapping vk_queue_family_mapping_init{
		vk_queue_family_mapping_invalid,
		vk_queue_family_mapping_invalid,
		vk_queue_family_mapping_invalid,
		vk_queue_family_mapping_invalid,
		vk_queue_family_mapping_invalid,
		vk_queue_family_mapping_invalid};

	struct vk_surface_capabilities
	{
		VkSurfaceCapabilitiesKHR		m_capabilities;
		std::vector<VkSurfaceFormatKHR> m_formats;
		std::vector<VkPresentModeKHR>	m_present_modes;
	};
} // namespace vvv
