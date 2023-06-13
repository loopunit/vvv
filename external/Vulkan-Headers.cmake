CPMAddPackage(
  NAME Vulkan-Headers
  GITHUB_REPOSITORY "KhronosGroup/Vulkan-Headers"
  GIT_TAG 0e89587db3ebee4d463f191bd296374c5fafc8ea
  DOWNLOAD_ONLY YES)

add_library(Vulkan-Headers INTERFACE)

#target_link_libraries(Vulkan-Headers PUBLIC Vulkan::Vulkan)

target_include_directories(Vulkan-Headers PUBLIC
    "$<BUILD_INTERFACE:${Vulkan-Headers_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_library(Vulkan::Headers ALIAS Vulkan-Headers)

target_compile_definitions(Vulkan::Headers INTERFACE VK_ENABLE_BETA_EXTENSIONS) # Enable beta Vulkan extensions

if(WIN32)
    target_compile_definitions(Vulkan::Headers INTERFACE VK_USE_PLATFORM_WIN32_KHR)
elseif(ANDROID)
    target_compile_definitions(Vulkan::Headers INTERFACE VK_USE_PLATFORM_ANDROID_KHR)
elseif(APPLE)
    target_compile_definitions(Vulkan::Headers INTERFACE VK_USE_PLATFORM_MACOS_MVK VK_USE_PLATFORM_METAL_EXT)
else()
    option(BUILD_WSI_XCB_SUPPORT "Build XCB WSI support" ON)
    option(BUILD_WSI_XLIB_SUPPORT "Build Xlib WSI support" ON)
    option(BUILD_WSI_WAYLAND_SUPPORT "Build Wayland WSI support" ON)

    #find_package(PkgConfig REQUIRED QUIET) # Use PkgConfig to find Linux system libraries

    if(BUILD_WSI_XCB_SUPPORT)
        #pkg_check_modules(XCB REQUIRED QUIET IMPORTED_TARGET xcb)
        target_compile_definitions(Vulkan::Headers INTERFACE VK_USE_PLATFORM_XCB_KHR)
    endif()

    if(BUILD_WSI_XLIB_SUPPORT)
        #pkg_check_modules(X11 REQUIRED QUIET IMPORTED_TARGET x11)
        target_compile_definitions(Vulkan::Headers INTERFACE VK_USE_PLATFORM_XLIB_KHR VK_USE_PLATFORM_XLIB_XRANDR_EXT)
    endif()

    if(BUILD_WSI_WAYLAND_SUPPORT)
        #pkg_check_modules(WAYlAND_CLIENT QUIET REQUIRED IMPORTED_TARGET wayland-client)
        target_compile_definitions(Vulkan::Headers INTERFACE VK_USE_PLATFORM_WAYLAND_KHR)
    endif()
endif()