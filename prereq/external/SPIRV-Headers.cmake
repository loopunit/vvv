CPMAddPackage(
  NAME SPIRV-Headers
  GITHUB_REPOSITORY "KhronosGroup/SPIRV-Headers"
  GIT_TAG 268a061764ee69f09a477a695bf6a11ffe311b8d
  OPTIONS
    "SPIRV_SKIP_TESTS ON")
  #DOWNLOAD_ONLY YES)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(DIRECTORY ${SPIRV-Headers_SOURCE_DIR}/include/spirv DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

set(cmake_install_dir "${CMAKE_INSTALL_DATADIR}/cmake/SPIRV-Headers")
set(version_config "${CMAKE_CURRENT_BINARY_DIR}/generated/SPIRV-HeadersConfigVersion.cmake")

write_basic_package_version_file("${version_config}" COMPATIBILITY SameMajorVersion ARCH_INDEPENDENT)
install(FILES "${version_config}" DESTINATION "${cmake_install_dir}")

install(TARGETS SPIRV-Headers EXPORT "SPIRV-HeadersConfig" INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(EXPORT "SPIRV-HeadersConfig" NAMESPACE "SPIRV-Headers::" DESTINATION "${cmake_install_dir}")

if (IS_ABSOLUTE ${CMAKE_INSTALL_INCLUDEDIR})
    set(SPIRV_HEADERS_PKGCONFIG_INCLUDE_DIR ${CMAKE_INSTALL_INCLUDEDIR})
else()
    set(SPIRV_HEADERS_PKGCONFIG_INCLUDE_DIR ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_INCLUDEDIR})
endif()
configure_file(${SPIRV-Headers_SOURCE_DIR}/cmake/SPIRV-Headers.pc.in ${CMAKE_CURRENT_BINARY_DIR}/SPIRV-Headers.pc @ONLY)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/SPIRV-Headers.pc" DESTINATION ${CMAKE_INSTALL_DATADIR}/pkgconfig)
