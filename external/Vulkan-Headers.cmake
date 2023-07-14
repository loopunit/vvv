CPMAddPackage(
  NAME Vulkan-Headers
  GITHUB_REPOSITORY "KhronosGroup/Vulkan-Headers"
  GIT_TAG 9e61870ecbd32514113b467e0a0c46f60ed222c7)
  #DOWNLOAD_ONLY YES)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(VLK_REGISTRY_DIR "${CMAKE_INSTALL_DATADIR}/vulkan")

install(DIRECTORY "${Vulkan-Headers_SOURCE_DIR}/include/vk_video" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY "${Vulkan-Headers_SOURCE_DIR}/include/vulkan" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# Preserve source permissions https://github.com/KhronosGroup/Vulkan-Headers/issues/336
install(DIRECTORY "${Vulkan-Headers_SOURCE_DIR}/registry" DESTINATION ${VLK_REGISTRY_DIR} USE_SOURCE_PERMISSIONS)

set(cmake_files_install_dir ${CMAKE_INSTALL_DATADIR}/cmake/VulkanHeaders/)

set_target_properties(Vulkan-Headers PROPERTIES EXPORT_NAME "Headers")

#

target_compile_definitions(Vulkan-Headers INTERFACE VK_ENABLE_BETA_EXTENSIONS) # Enable beta Vulkan extensions

if(WIN32)
    target_compile_definitions(Vulkan-Headers INTERFACE VK_USE_PLATFORM_WIN32_KHR)
elseif(ANDROID)
    target_compile_definitions(Vulkan-Headers INTERFACE VK_USE_PLATFORM_ANDROID_KHR)
elseif(APPLE)
    target_compile_definitions(Vulkan-Headers INTERFACE VK_USE_PLATFORM_MACOS_MVK VK_USE_PLATFORM_METAL_EXT)
else()
    option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
    option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
    option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)

    #find_package(PkgConfig REQUIRED QUIET) # Use PkgConfig to find Linux system libraries

    if(BUILD_WSI_XCB_SUPPORT)
        #pkg_check_modules(XCB REQUIRED QUIET IMPORTED_TARGET xcb)
        target_compile_definitions(Vulkan-Headers INTERFACE VK_USE_PLATFORM_XCB_KHR)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        #pkg_check_modules(X11 REQUIRED QUIET IMPORTED_TARGET x11)
        target_compile_definitions(Vulkan-Headers INTERFACE VK_USE_PLATFORM_XLIB_KHR VK_USE_PLATFORM_XLIB_XRANDR_EXT)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        #pkg_check_modules(WAYlAND_CLIENT QUIET REQUIRED IMPORTED_TARGET wayland-client)
        target_compile_definitions(Vulkan-Headers INTERFACE VK_USE_PLATFORM_WAYLAND_KHR)
    endif()
endif()
install(TARGETS Vulkan-Headers EXPORT VulkanHeadersTargets INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

#

install(EXPORT VulkanHeadersTargets FILE VulkanHeadersTargets.cmake NAMESPACE "Vulkan::" DESTINATION ${cmake_files_install_dir})

set(vulkan_headers_config "${CMAKE_CURRENT_BINARY_DIR}/VulkanHeadersConfig.cmake")
set(VULKAN_HEADERS_REGISTRY_DIRECTORY "${VLK_REGISTRY_DIR}/registry")

configure_package_config_file(${Vulkan-Headers_SOURCE_DIR}/cmake/VulkanHeadersConfig.cmake.in ${vulkan_headers_config}
	INSTALL_DESTINATION ${cmake_files_install_dir}
	PATH_VARS VULKAN_HEADERS_REGISTRY_DIRECTORY
	NO_SET_AND_CHECK_MACRO
	NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

function(vlk_get_header_version)
    set(vulkan_core_header_file "${Vulkan-Headers_SOURCE_DIR}/include/vulkan/vulkan_core.h")
    if (NOT EXISTS ${vulkan_core_header_file})
        message(FATAL_ERROR "Couldn't find vulkan_core.h!")
    endif()

    file(READ ${vulkan_core_header_file} ver)

    if (ver MATCHES "#define[ ]+VK_HEADER_VERSION_COMPLETE[ ]+VK_MAKE_API_VERSION\\([ ]*[0-9]+,[ ]*([0-9]+),[ ]*([0-9]+),[ ]*VK_HEADER_VERSION[ ]*\\)")
        set(MAJOR_VERSION "${CMAKE_MATCH_1}")
        set(MINOR_VERSION "${CMAKE_MATCH_2}")
    else()
        message(FATAL_ERROR "Couldn't get major/minor version")
    endif()

    if (ver MATCHES "#define[ ]+VK_HEADER_VERSION[ ]+([0-9]+)")
        set(PATCH_VERSION "${CMAKE_MATCH_1}")
    else()
        message(FATAL_ERROR "Couldn't get the patch version")
    endif()

    set(VK_VERSION_STRING "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}" PARENT_SCOPE)
endfunction()
vlk_get_header_version()

project(VULKAN_HEADERS LANGUAGES C VERSION ${VK_VERSION_STRING})

set(config_version "${CMAKE_CURRENT_BINARY_DIR}/VulkanHeadersConfigVersion.cmake")

write_basic_package_version_file(${config_version} VERSION ${VK_VERSION_STRING} COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)

install(FILES ${config_version} ${vulkan_headers_config} DESTINATION ${cmake_files_install_dir})
