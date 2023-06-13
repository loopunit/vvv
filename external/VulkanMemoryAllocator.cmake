CPMAddPackage(
  NAME VulkanMemoryAllocator
  GITHUB_REPOSITORY "GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator"
  GIT_TAG 0e89587db3ebee4d463f191bd296374c5fafc8ea
  DOWNLOAD_ONLY YES)

set(VULKANMA_LIBRARY_SOURCE_FILES
    "${VulkanMemoryAllocator_SOURCE_DIR}/src/VmaUsage.cpp"
    "${VulkanMemoryAllocator_SOURCE_DIR}/src/VmaUsage.h"
    "${VulkanMemoryAllocator_SOURCE_DIR}/include/vk_mem_alloc.h"
)

add_library(VulkanMemoryAllocator ${VULKANMA_LIBRARY_SOURCE_FILES})

if (MSVC)
    # Provides MSVC users nicer debugging support
    target_sources(VulkanMemoryAllocator PRIVATE ${VulkanMemoryAllocator_SOURCE_DIR}/src/vk_mem_alloc.natvis)
endif()

target_link_libraries(VulkanMemoryAllocator PUBLIC Vulkan::Headers)

target_include_directories(VulkanMemoryAllocator PUBLIC
    "$<BUILD_INTERFACE:${VulkanMemoryAllocator_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_library(Vulkan::MemoryAllocator ALIAS VulkanMemoryAllocator)