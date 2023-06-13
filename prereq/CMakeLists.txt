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
	set(ENV{CPM_SOURCE_CACHE} "${CMAKE_CURRENT_LIST_DIR}/.cpm")
else()
	set(ENV{CPM_SOURCE_CACHE} ${vvv_ROOT_DIR}/.cpm)
endif()

include("${CMAKE_CURRENT_LIST_DIR}/external/CPM.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/external/Vulkan-Headers.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/external/VulkanMemoryAllocator.cmake")
include("${CMAKE_CURRENT_LIST_DIR}/external/DirectXShaderCompiler.cmake")
#include("${CMAKE_CURRENT_LIST_DIR}/external/SPIRV-Headers.cmake")
#include("${CMAKE_CURRENT_LIST_DIR}/external/SPIRV-Tools.cmake")
#include("${CMAKE_CURRENT_LIST_DIR}/external/Vulkan-ValidationLayers.cmake")
#include("${CMAKE_CURRENT_LIST_DIR}/external/SPIRV-Cross.cmake")
#include("${CMAKE_CURRENT_LIST_DIR}/external/SPIRV-Reflect.cmake")
