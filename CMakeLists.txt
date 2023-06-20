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

cmake_path(GET CMAKE_INSTALL_PREFIX STEM LAST_ONLY vvv_TARGET_ARCH)
set(vvv_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})
set(vvv_PREREQ_ROOT ${CMAKE_CURRENT_LIST_DIR}/.local/prereq)
set(vvv_PREREQ_TARGET ${vvv_PREREQ_ROOT}/${vvv_TARGET_ARCH})
set(vvv_PREREQ_INSTALL ${vvv_PREREQ_TARGET}/install)

if (${vvv_PREREQ})
	ExternalProject_Add(vvv_prereq_dependencies
	  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}/prereq
	  BINARY_DIR ${vvv_PREREQ_TARGET}/bin
	  TMP_DIR ${vvv_PREREQ_TARGET}/tmp
	  STAMP_DIR ${vvv_PREREQ_TARGET}/stamp
	  LOG_DIR ${vvv_PREREQ_TARGET}/log
	  DOWNLOAD_DIR ${vvv_PREREQ_TARGET}/download
	  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${vvv_PREREQ_INSTALL} -Dvvv_ROOT_DIR:PATH=${vvv_ROOT_DIR}
	  INSTALL_DIR ${vvv_PREREQ_INSTALL}
	  UPDATE_COMMAND ;
	  USES_TERMINAL_DOWNLOAD YES
	  USES_TERMINAL_PATCH YES
	  USES_TERMINAL_CONFIGURE YES
	  USES_TERMINAL_BUILD YES
	  USES_TERMINAL_INSTALL YES
	  USES_TERMINAL_UPDATE YES
	  USES_TERMINAL_TEST YES)

	add_custom_target(build_vvv_prereq)
	add_dependencies(build_vvv_prereq vvv_prereq_dependencies)
endif()

if (NOT ${vvv_PREREQ})
	function(vvv_find_package NAME)
		find_package(${NAME} PATHS ${vvv_PREREQ_INSTALL})
	endfunction()

	add_library(vvv STATIC "src/vvv.cpp")

	vvv_find_package(VulkanHeaders)
	find_package(boost_leaf REQUIRED)
	find_package(fmt REQUIRED)
	find_package(WIL REQUIRED)
	find_package(concurrentqueue REQUIRED)
	find_package(libcuckoo REQUIRED)
	find_package(Taskflow REQUIRED)

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
			Vulkan::Headers
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