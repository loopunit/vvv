if (NOT DEFINED vvv_ROOT_DIR)
	get_filename_component(vvv_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()

set(ENV{CPM_SOURCE_CACHE} "${vvv_ROOT_DIR}/.cpm")

cmake_path(GET CMAKE_INSTALL_PREFIX STEM LAST_ONLY vvv_TARGET_ARCH)

set(vvv_PREREQ_ROOT ${vvv_ROOT_DIR}/.local/prereq)
set(vvv_PREREQ_TARGET ${vvv_PREREQ_ROOT}/${vvv_TARGET_ARCH})
set(vvv_PREREQ_INSTALL ${vvv_PREREQ_TARGET}/install)

list(APPEND CMAKE_PREFIX_PATH "${vvv_PREREQ_INSTALL}")

#function(vvv_find_package NAME)
#	find_package(${NAME} PATHS ${vvv_PREREQ_INSTALL})
#endfunction()

if (${vvv_PREREQ})
	ExternalProject_Add(vvv_prereq_layer_0_dependencies
	  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}
	  BINARY_DIR ${vvv_PREREQ_TARGET}/layer_0/bin
	  TMP_DIR ${vvv_PREREQ_TARGET}/layer_0/tmp
	  STAMP_DIR ${vvv_PREREQ_TARGET}/layer_0/stamp
	  LOG_DIR ${vvv_PREREQ_TARGET}/layer_0/log
	  DOWNLOAD_DIR ${vvv_PREREQ_TARGET}/layer_0/download
	  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${vvv_PREREQ_INSTALL} -Dvvv_ROOT_DIR:PATH=${vvv_ROOT_DIR} -Dvvv_PREREQ_LAYER:STRING=0
	  INSTALL_DIR ${vvv_PREREQ_INSTALL}
	  UPDATE_COMMAND ;
	  USES_TERMINAL_DOWNLOAD YES
	  USES_TERMINAL_PATCH YES
	  USES_TERMINAL_CONFIGURE YES
	  USES_TERMINAL_BUILD YES
	  USES_TERMINAL_INSTALL YES
	  USES_TERMINAL_UPDATE YES
	  USES_TERMINAL_TEST YES)

	add_custom_target(build_vvv_prereq_layer_0)
	add_dependencies(build_vvv_prereq_layer_0 vvv_prereq_layer_0_dependencies)

	ExternalProject_Add(vvv_prereq_layer_1_dependencies
	  SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR}
	  BINARY_DIR ${vvv_PREREQ_TARGET}/layer_1/bin
	  TMP_DIR ${vvv_PREREQ_TARGET}/layer_1/tmp
	  STAMP_DIR ${vvv_PREREQ_TARGET}/layer_1/stamp
	  LOG_DIR ${vvv_PREREQ_TARGET}/layer_1/log
	  DOWNLOAD_DIR ${vvv_PREREQ_TARGET}/layer_1/download
	  CMAKE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${vvv_PREREQ_INSTALL} -Dvvv_ROOT_DIR:PATH=${vvv_ROOT_DIR} -Dvvv_PREREQ_LAYER:STRING=1
	  INSTALL_DIR ${vvv_PREREQ_INSTALL}
	  UPDATE_COMMAND ;
	  USES_TERMINAL_DOWNLOAD YES
	  USES_TERMINAL_PATCH YES
	  USES_TERMINAL_CONFIGURE YES
	  USES_TERMINAL_BUILD YES
	  USES_TERMINAL_INSTALL YES
	  USES_TERMINAL_UPDATE YES
	  USES_TERMINAL_TEST YES)

	add_custom_target(build_vvv_prereq_layer_1)
	add_dependencies(build_vvv_prereq_layer_1 build_vvv_prereq_layer_0 vvv_prereq_layer_1_dependencies)
endif()