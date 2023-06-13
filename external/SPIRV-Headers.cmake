CPMAddPackage(
  NAME SPIRV-Headers
  GITHUB_REPOSITORY "KhronosGroup/SPIRV-Headers"
  GIT_TAG 8e2ad27488ed2f87c068c01a8f5e8979f7086405
  DOWNLOAD_ONLY YES)

add_library(SPIRV-Headers INTERFACE)

target_link_libraries(SPIRV-Headers PUBLIC Vulkan::Headers)

target_include_directories(SPIRV-Headers PUBLIC
    "$<BUILD_INTERFACE:${SPIRV-Headers_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_library(SPIRV-Headers::SPIRV-Headers ALIAS SPIRV-Headers)
