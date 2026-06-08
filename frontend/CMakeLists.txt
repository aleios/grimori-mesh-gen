project(mesh-gen)

# Executable front end
add_executable(mesh-gen main.cpp)
target_link_libraries(mesh-gen PRIVATE meshgenlib argparse)

set_target_properties(mesh-gen PROPERTIES
                      RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)