CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
PROJECT(GPUPixelProject)

# Add src directory first
ADD_SUBDIRECTORY(src)

# Add demo directory and set dependencies
ADD_SUBDIRECTORY(demo)
# Get all targets from demo directory
get_directory_property(EXAMPLE_TARGETS DIRECTORY demo BUILDSYSTEM_TARGETS)
# Add src dependency for each example target
foreach(TARGET ${EXAMPLE_TARGETS})
    add_dependencies(${TARGET} gpupixel)
endforeach()