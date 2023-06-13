CPMAddPackage(
  NAME Polypropylene
  GITHUB_REPOSITORY "pmbittner/Polypropylene"
  GIT_TAG 1e1ff4fbf9fef735b34dfe444bdfc941fb7df26c
DOWNLOAD_ONLY YES)

set(Polypropylene_LIBRARY_SOURCE_FILES
        "${Polypropylene_SOURCE_DIR}/src/event/Event.cpp"
        "${Polypropylene_SOURCE_DIR}/src/event/EventService.cpp"

        "${Polypropylene_SOURCE_DIR}/src/io/Path.cpp"

        "${Polypropylene_SOURCE_DIR}/src/property/Entity.cpp"
        "${Polypropylene_SOURCE_DIR}/src/property/Prefab.cpp"

        "${Polypropylene_SOURCE_DIR}/src/serialisation/ClassMetadataSerialiser.cpp"
        "${Polypropylene_SOURCE_DIR}/src/serialisation/FieldStorage.cpp"

        "${Polypropylene_SOURCE_DIR}/src/stdutils/StringUtils.cpp"

        "${Polypropylene_SOURCE_DIR}/src/log/Log.cpp"

        "${Polypropylene_SOURCE_DIR}/src/memory/Allocator.cpp"
        "${Polypropylene_SOURCE_DIR}/src/memory/AllocationService.cpp"
        "${Polypropylene_SOURCE_DIR}/src/memory/PropertyPool.cpp"
        "${Polypropylene_SOURCE_DIR}/src/memory/allocators/MallocAllocator.cpp"
        "${Polypropylene_SOURCE_DIR}/src/memory/allocators/PoolAllocator.cpp"

        "${Polypropylene_SOURCE_DIR}/src/reflection/ClassMetadata.cpp"
        "${Polypropylene_SOURCE_DIR}/src/reflection/Field.cpp"
        "${Polypropylene_SOURCE_DIR}/src/reflection/VariableRegister.cpp"
        "${Polypropylene_SOURCE_DIR}/src/reflection/Type.cpp")

add_library(Polypropylene ${Polypropylene_LIBRARY_SOURCE_FILES})

set_target_properties(Polypropylene
    PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF)

target_include_directories(Polypropylene PUBLIC
    "$<BUILD_INTERFACE:${Polypropylene_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")

add_library(Polypropylene::Polypropylene ALIAS Polypropylene)