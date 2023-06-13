CPMAddPackage(
  NAME boost_leaf
  GITHUB_REPOSITORY "boostorg/leaf"
  GIT_TAG 801e5e813ff9923d8d610b1b00597226369328d1
  OPTIONS
    "BUILD_TESTING OFF")

if (boost_leaf_INSTALL)
	include(CMakePackageConfigHelpers)

	install(TARGETS boost_leaf
			EXPORT boost_leaf-Targets
			ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

	install(EXPORT boost_leaf-Targets FILE boost_leaf-targets.cmake
			NAMESPACE Boost::
			DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/boost_leaf/cmake)

	install(DIRECTORY "${boost_leaf_SOURCE_DIR}/include/"
			DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

	write_basic_package_version_file("boost_leaf-config-version.cmake"
									VERSION ${PROJECT_VERSION}
									COMPATIBILITY SameMajorVersion)
	configure_package_config_file(
		"${CMAKE_CURRENT_LIST_DIR}/cmake/boost_leaf-config.cmake.in"
		"${CMAKE_CURRENT_BINARY_DIR}/boost_leaf-config.cmake"
		INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/boost_leaf/cmake)

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/boost_leaf-config.cmake"
				"${CMAKE_CURRENT_BINARY_DIR}/boost_leaf-config-version.cmake"
			DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/boost_leaf/cmake)
endif()