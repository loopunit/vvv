CPMAddPackage(
  NAME Vulkan-ValidationLayers
  GITHUB_REPOSITORY "KhronosGroup/Vulkan-ValidationLayers"
  GIT_TAG ff7d97ac15dc038cb6caba9b9d7a55a3b934688b
  DOWNLOAD_ONLY YES)

add_subdirectory(${Vulkan-ValidationLayers_SOURCE_DIR}/scripts)

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

set(BUILD_LAYERS ON)
add_subdirectory(${Vulkan-ValidationLayers_SOURCE_DIR}/layers)

add_library(Vulkan-ValidationLayers INTERFACE)

target_link_libraries(Vulkan-ValidationLayers PUBLIC Vulkan::Vulkan)

target_include_directories(Vulkan-ValidationLayers PUBLIC
    "$<BUILD_INTERFACE:${Vulkan-ValidationLayers_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_library(Vulkan::ValidationLayers ALIAS Vulkan-ValidationLayers)