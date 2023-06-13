set(Vulkan-ValidationLayers_PATCHFILE ${CMAKE_CURRENT_LIST_DIR}/Vulkan-ValidationLayers.patch)
set(Vulkan-ValidationLayers_PATCH_COMMAND git apply ${Vulkan-ValidationLayers_PATCHFILE})

if (EXISTS ${Vulkan-ValidationLayers_PATCHFILE})
	set(Vulkan-ValidationLayers_EXTRA_ARGS PATCH_COMMAND ${Vulkan-ValidationLayers_PATCH_COMMAND})
endif()

CPMAddPackage(
  NAME Vulkan-ValidationLayers
  GITHUB_REPOSITORY "KhronosGroup/Vulkan-ValidationLayers"
  GIT_TAG 1541e00a63cd125f15d231d5a8059ebe66503b25
  ${Vulkan-ValidationLayers_EXTRA_ARGS}
  DOWNLOAD_ONLY YES)

#message(STATUS "Generate the patch thusly:")
#message(STATUS "cd ${Vulkan-ValidationLayers_SOURCE_DIR}")
#message(STATUS "git diff > ${Vulkan-ValidationLayers_PATCHFILE}")

add_subdirectory(${Vulkan-ValidationLayers_SOURCE_DIR}/scripts)

set(VVL_CPP_STANDARD 17 CACHE STRING "Set the C++ standard to build against.")
set(CMAKE_CXX_STANDARD ${VVL_CPP_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN "YES")

include(GNUInstallDirs)

set_property(GLOBAL PROPERTY USE_FOLDERS ON) # Remove when min is 3.26, see CMP0143

if (ANNOTATED_SPEC_LINK)
    message("ANNOTATED_SPEC_LINK is ${ANNOTATED_SPEC_LINK}")
    add_compile_definitions(ANNOTATED_SPEC_LINK=${ANNOTATED_SPEC_LINK})
endif()

option(BUILD_WERROR "Treat compiler warnings as errors")
if (BUILD_WERROR)
    add_compile_options("$<IF:$<CXX_COMPILER_ID:MSVC>,/WX,-Werror>")
endif()

option(VVL_ENABLE_ASAN "Use address sanitization")
if (VVL_ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
endif()

option(VVL_CLANG_TIDY "Use clang-tidy")
if (VVL_CLANG_TIDY)
    find_program(CLANG_TIDY NAMES clang-tidy)
    if(NOT CLANG_TIDY)
        message(FATAL_ERROR "clang-tidy not found!")
    endif()
    set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY}")
endif()

if(${CMAKE_CXX_COMPILER_ID} MATCHES "(GNU|Clang)")
    add_compile_options(
        -Wall
        -Wextra
        -Wpointer-arith
    )
    if(${CMAKE_CXX_COMPILER_ID} MATCHES "Clang")
        add_compile_options(
            -Wconversion
            -Wimplicit-fallthrough
            -Wstring-conversion
        )
    endif()
elseif(MSVC)
    add_compile_options(
        /W4
        /we5038 # Enable warning about MIL ordering in constructors

        # Disable warnings
        /wd4324 # padding
        /wd4458 # hiding class member
        /wd4457 # hiding function parameter
        /wd4459 # hiding global parameter
        /wd4702 # unreachable code
        /wd4389 # signed/unsigned mismatch
        /wd4754 # conversion rules for arithmetic operations
        /wd4706 # assignment within conditional expression
    )

    # Enforce stricter ISO C++
    add_compile_options(/permissive-)

    # PDBs aren't generated on Release builds by default.
    add_compile_options("$<$<CONFIG:Release>:/Zi>")
    add_link_options("$<$<CONFIG:Release>:/DEBUG:FULL>")

    # Enable /LTCG (Link-time code generation)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    # Remove unreferenced code/data.
    add_link_options("$<$<CONFIG:Release>:/OPT:REF>")
    # Merge duplicate data/functions
    add_link_options("$<$<CONFIG:Release>:/OPT:ICF>")

    # Allow usage of unsafe CRT functions and minimize what Windows.h leaks
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS NOMINMAX WIN32_LEAN_AND_MEAN)

    add_compile_options($<$<BOOL:${MSVC_IDE}>:/MP>) # Speed up Visual Studio builds
endif()

option(BUILD_LAYERS "Build validation layers" ON)
option(BUILD_LAYER_SUPPORT_FILES "Install VkLayer_utils")

if(BUILD_LAYERS OR BUILD_LAYER_SUPPORT_FILES)
    add_subdirectory(${Vulkan-ValidationLayers_SOURCE_DIR}/layers)
endif()

#option(BUILD_TESTS "Build the tests")
#if(BUILD_TESTS)
#    enable_testing()
#    add_subdirectory(${Vulkan-ValidationLayers_SOURCE_DIR}/tests)
#endif()
