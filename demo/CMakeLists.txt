# Basic Project Configuration
# ----------------------------------------
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 11)

# Set cmake module path to current directory for finding custom modules
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# Define and initialize application project
set(APP_PROJECT_NAME "gpupixel_app")
project(${APP_PROJECT_NAME})

# Platform Detection
# Detect and set the current operating system
# ----------------------------------------
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CURRENT_OS "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CURRENT_OS "windows")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CURRENT_OS "macos")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    set(CURRENT_OS "ios")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    set(CURRENT_OS "android")
else()
    message(FATAL_ERROR "NOT SUPPORT THIS SYSTEM")
endif()

# Output Directory Configuration
# Configure paths for different build artifacts
# ----------------------------------------
set(OUTPUT_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output")
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/include")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/app/${CURRENT_OS}")

# Set output directories for different build configurations (Debug/Release)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Resource and Runtime Directory Configuration
# ----------------------------------------
link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})

# Include Directories Configuration
# Add all necessary include paths for header files
# ----------------------------------------
include_directories(
    # Project headers
    ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
    # Third-party libraries
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glad/include
)

# Source Files Configuration
# ----------------------------------------
# Add desktop application source files
file(GLOB SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/desktop/*"
)

# Platform Specific Configuration
# Add platform-specific source files and settings
# ----------------------------------------
if(${CURRENT_OS} STREQUAL "windows")
    # Configure MSVC runtime library
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
    
    # Add Windows-specific library paths
    link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glfw/lib-mingw-w64)

    # Add GLAD source files for Windows
    file(GLOB GLAD_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glad/src/*.c")
    list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})
elseif(${CURRENT_OS} STREQUAL "linux")
   
endif()

# Build Configuration
# ----------------------------------------
# Create executable from source files
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Library Linkage Configuration
# Configure platform-specific library dependencies
# ----------------------------------------
if(${CURRENT_OS} STREQUAL "linux")
    # Linux dependencies
    target_link_libraries(${PROJECT_NAME}
                        gpupixel
                        GL
                        glfw)
    set(DEPS_LIBS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libgpupixel.so
                      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libmars-face-kit.so)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./")
elseif(${CURRENT_OS} STREQUAL "windows")
    # Windows dependencies
    set(DEPS_LIBS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/gpupixel.dll
                      ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/mars-face-kit.dll)

    target_link_libraries(${PROJECT_NAME}
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/gpupixel.lib
                        opengl32
                        ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glfw/libs/msvc-x64/glfw3.lib)

    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./")
endif()
 
# Post-Build Commands
# Copy necessary resources and libraries to output directory
# ----------------------------------------
# Copy resource and model files
set(COPY_DST_RUNTIME_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/res ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res

    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/../output/models ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models

    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/desktop/demo.png" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}
    
    COMMENT "Copying res and models files to output/app directory.")

# Copy demo image and libraries
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${DEPS_LIBS} ${COPY_DST_RUNTIME_DIR}
    COMMENT "Copying libraries to output directory.")
