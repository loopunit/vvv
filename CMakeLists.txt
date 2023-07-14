cmake_minimum_required(VERSION 3.23)

project(vvv
	LANGUAGES CXX
	VERSION 0.0.0)

include(GNUInstallDirs)
include(ExternalProject)
	
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(vvv_PREREQ "Prerequisite installation logic" ${IS_TOPLEVEL_PROJECT})
option(vvv_INSTALL "Installation logic" FALSE)

if (${IS_TOPLEVEL_PROJECT} AND NOT DEFINED vvv_ROOT_DIR)
	get_filename_component(vvv_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}" ABSOLUTE)
endif()

include("prereq/prereq.cmake")

if (NOT ${vvv_PREREQ})
	add_library(vvv STATIC 
		"src/vvv.cpp"
		"include/vvv.h"
		"include/vvv_window.h")

	find_package(volk REQUIRED)
	find_package(boost_leaf REQUIRED)
	find_package(fmt REQUIRED)
	find_package(WIL REQUIRED)
	find_package(concurrentqueue REQUIRED)
	find_package(libcuckoo REQUIRED)
	find_package(Taskflow REQUIRED)
	find_package(VulkanHeaders REQUIRED)
	find_package(VulkanMemoryAllocator REQUIRED)

	target_include_directories(vvv PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

	target_compile_definitions(vvv PUBLIC NOMINMAX)

	set_target_properties(vvv
		PROPERTIES
			CXX_STANDARD 20
			CXX_EXTENSIONS OFF)

	target_link_libraries(vvv
		PUBLIC
			Boost::leaf
			fmt::fmt	
			WIL::WIL
			concurrentqueue
			libcuckoo::libcuckoo
			volk::volk
			VulkanMemoryAllocator
			#Polypropylene::Polypropylene
			Taskflow)

	add_library(loopunit::vvv ALIAS vvv)

	if (vvv_INSTALL)
		include(CMakePackageConfigHelpers)

		install(TARGETS vvv
				EXPORT vvv-Targets
				ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})

		install(EXPORT vvv-Targets FILE vvv-targets.cmake
				NAMESPACE loopunit::
				DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/vvv/cmake)

		install(DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/include/"
				DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

		write_basic_package_version_file("vvv-config-version.cmake"
										VERSION ${PROJECT_VERSION}
										COMPATIBILITY SameMajorVersion)
		configure_package_config_file(
			"${CMAKE_CURRENT_LIST_DIR}/cmake/vvv-config.cmake.in"
			"${CMAKE_CURRENT_BINARY_DIR}/vvv-config.cmake"
			INSTALL_DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/vvv/cmake)

		install(FILES "${CMAKE_CURRENT_BINARY_DIR}/vvv-config.cmake"
					"${CMAKE_CURRENT_BINARY_DIR}/vvv-config-version.cmake"
				DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/vvv/cmake)
	endif()
endif()