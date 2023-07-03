cmake_minimum_required(VERSION 3.23)

project(vvv_testing)

include(GNUInstallDirs)
	
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(IS_TOPLEVEL_PROJECT TRUE)
else()
    set(IS_TOPLEVEL_PROJECT FALSE)
endif()

if (${IS_TOPLEVEL_PROJECT} AND NOT DEFINED vvv_ROOT_DIR)
	get_filename_component(vvv_ROOT_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
endif()

include("${vvv_ROOT_DIR}/prereq/prereq.cmake")

include("${vvv_ROOT_DIR}/external/CPM.cmake")
include("${vvv_ROOT_DIR}/external/catch2.cmake")
include("${vvv_ROOT_DIR}/external/cmrc.cmake")
include("${vvv_ROOT_DIR}/external/wil.cmake")
include("${vvv_ROOT_DIR}/external/leaf.cmake")
include("${vvv_ROOT_DIR}/external/fmt.cmake")
include("${vvv_ROOT_DIR}/external/winlamb.cmake")
include("${vvv_ROOT_DIR}/external/concurrentqueue.cmake")
include("${vvv_ROOT_DIR}/external/libcuckoo.cmake")
include("${vvv_ROOT_DIR}/external/taskflow.cmake")
include("${vvv_ROOT_DIR}/external/Polypropylene.cmake")
include("${vvv_ROOT_DIR}/external/volk.cmake")

add_subdirectory("${vvv_ROOT_DIR}/" "${CMAKE_CURRENT_BINARY_DIR}/vvv")

add_executable(vvv_testing main.cpp)

set_target_properties(vvv_testing
    PROPERTIES COMPILE_FLAGS
        "/Zc:__cplusplus")

set_target_properties(vvv_testing
    PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF)

target_link_libraries(vvv_testing
	PUBLIC
		loopunit::vvv
        Catch2::Catch2WithMain
        winlamb::winlamb)

add_custom_command(TARGET vvv_testing POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_if_different
               "${vvv_PREREQ_INSTALL}/bin/VkLayer_khronos_validation.dll"              
                $<TARGET_FILE_DIR:vvv_testing>)
				
add_custom_command(TARGET vvv_testing POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_if_different
               "${vvv_PREREQ_INSTALL}/bin/VkLayer_khronos_validation.json"
                $<TARGET_FILE_DIR:vvv_testing>)

add_custom_command(TARGET vvv_testing POST_BUILD 
               COMMAND ${CMAKE_COMMAND} -E copy_if_different
               "${vvv_PREREQ_INSTALL}/bin/VkLayer_khronos_validation.pdb"
                $<TARGET_FILE_DIR:vvv_testing>)