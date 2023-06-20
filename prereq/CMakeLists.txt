cmake_minimum_required(VERSION 3.23)

project(vvv_prereq
	LANGUAGES CXX
	VERSION 0.0.0)

include(GNUInstallDirs)
	
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

option(vvv_prereq_INSTALL "Installation logic" ${IS_TOPLEVEL_PROJECT})

if (NOT DEFINED vvv_ROOT_DIR)
	get_filename_component(PARENT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
	set(ENV{CPM_SOURCE_CACHE} "${PARENT_DIR}/.cpm")
	set(vvv_ROOT_DIR ${PARENT_DIR})
else()
	set(ENV{CPM_SOURCE_CACHE} ${vvv_ROOT_DIR}/.cpm)
endif()

include("${vvv_ROOT_DIR}/external/CPM.cmake")
include("${vvv_ROOT_DIR}/external/boost.cmake")
include("${vvv_ROOT_DIR}/external/Vulkan-Headers.cmake")
include("${vvv_ROOT_DIR}/external/VulkanMemoryAllocator.cmake")
include("${vvv_ROOT_DIR}/external/DirectXShaderCompiler.cmake")
include("${vvv_ROOT_DIR}/external/SDL.cmake")
