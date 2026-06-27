cmake_minimum_required(VERSION 4.2)
project(grimori-mesh-gen)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Opts
option(GMESHGEN_ENABLE_INSTALL "Enable installation of the library" ON)
option(GMESHGEN_ENABLE_TOOL "Enable frontend tool compilation" ON)

# Dependencies
set(ARGPARSE_INSTALL OFF CACHE BOOL "" FORCE)
set(FASTGLTF_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(MESHOPT_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(externals/argparse)
add_subdirectory(externals/fastgltf)
add_subdirectory(externals/meshoptimizer)

# Backing library
add_library(meshgenlib STATIC
            src/meshwriter.cpp
            src/gltfparser.cpp
            src/meshoperations.cpp
            src/meshpostprocessor.cpp

            src/meshreader.cpp
)

set_target_properties(meshgenlib PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    OUTPUT_NAME "meshgen"
)
target_link_libraries(meshgenlib PRIVATE fastgltf meshoptimizer)

target_include_directories(meshgenlib
   PUBLIC
       $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
       $<INSTALL_INTERFACE:include>
   PRIVATE
       ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(GMESHGEN_ENABLE_TOOL)
    add_subdirectory(frontend)
endif()

if(GMESHGEN_ENABLE_INSTALL)
    install(TARGETS mesh-gen
        RUNTIME DESTINATION bin
    )

    install(TARGETS meshgenlib
        ARCHIVE DESTINATION lib
    )

    install(DIRECTORY include/
            DESTINATION include
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
    )
endif()