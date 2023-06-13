CPMAddPackage(
  NAME SPIRV-Tools
  GITHUB_REPOSITORY "KhronosGroup/SPIRV-Tools"
  GIT_TAG 8e2ad27488ed2f87c068c01a8f5e8979f7086405
  DOWNLOAD_ONLY YES)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  add_definitions(-DSPIRV_LINUX)
  set(SPIRV_TIMER_ENABLED ${SPIRV_ALLOW_TIMERS})
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Emscripten")
    add_definitions(-DSPIRV_EMSCRIPTEN)
elseif("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
  add_definitions(-DSPIRV_WINDOWS)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "CYGWIN")
  add_definitions(-DSPIRV_WINDOWS)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  add_definitions(-DSPIRV_MAC)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
  add_definitions(-DSPIRV_IOS)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "tvOS")
  add_definitions(-DSPIRV_TVOS)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Android")
  add_definitions(-DSPIRV_ANDROID)
  set(SPIRV_TIMER_ENABLED ${SPIRV_ALLOW_TIMERS})
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "FreeBSD")
  add_definitions(-DSPIRV_FREEBSD)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "OpenBSD")
  add_definitions(-DSPIRV_OPENBSD)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Fuchsia")
  add_definitions(-DSPIRV_FUCHSIA)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "GNU")
  add_definitions(-DSPIRV_GNU)
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "QNX")
  add_definitions(-DSPIRV_QNX)
else()
  message(FATAL_ERROR "Your platform '${CMAKE_SYSTEM_NAME}' is not supported!")
endif()

add_library(SPIRV-Tools INTERFACE)

target_link_libraries(SPIRV-Tools PUBLIC SPIRV-Headers::SPIRV-Headers)

target_include_directories(SPIRV-Tools PUBLIC
    "$<BUILD_INTERFACE:${SPIRV-Tools_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_library(SPIRV-Tools::SPIRV-Tools ALIAS SPIRV-Tools)
