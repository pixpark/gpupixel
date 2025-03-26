# Basic Project Configuration
# ----------------------------------------
cmake_minimum_required(VERSION 3.10)

# Set C++ standard and compiler options
set(CMAKE_CXX_STANDARD 11)
#set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Set cmake module path to current directory
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# Define project name and initialize project
set(PROJECT_NAME "gpupixel")
project(${PROJECT_NAME})

# Define API export macros for Windows DLL exports
add_definitions(-DMYMATH_EXPORT_LIBRARY)

# Platform Detection
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
# ----------------------------------------
set(OUTPUT_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output")
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/include")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")

# Configure output directories for Debug/Release builds
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Source Files Configuration
# ----------------------------------------
# Core project headers
include_directories(
    # Project core modules
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/filter
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/sink
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/face_detect
    
    # Platform specific headers
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni
    ${CMAKE_CURRENT_SOURCE_DIR}/sink/objc
    
    # Third-party dependencies
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libyuv/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/include
)

# Collect source files
file(GLOB SOURCE_FILES     
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*"        
    "${CMAKE_CURRENT_SOURCE_DIR}/filter/*"         
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*"       
    "${CMAKE_CURRENT_SOURCE_DIR}/sink/*"                               
    "${CMAKE_CURRENT_SOURCE_DIR}/face_detect/*"                 
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*"                 
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libyuv/source/*"
)

# Collect public headers for export
file(GLOB EXPORT_HEADER 
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.h"         
    "${CMAKE_CURRENT_SOURCE_DIR}/filter/*.h"         
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"       
    "${CMAKE_CURRENT_SOURCE_DIR}/sink/*.h"                      
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.h"                 
    "${CMAKE_CURRENT_SOURCE_DIR}/face_detect/*.h"                 
)

# Collect resource files
file(GLOB RESOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/assets/*"                      
)

# Platform Specific Configuration
# ----------------------------------------
if(${CURRENT_OS} STREQUAL "windows")     
    set(CMAKE_SHARED_LIBRARY_PREFIX "")
    file(GLOB GLAD_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/*.c" )
    list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})
elseif(${CURRENT_OS} STREQUAL "linux")    
    file(GLOB GLAD_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/*.c" )
    list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})
elseif(${CURRENT_OS} STREQUAL "macos" OR ${CURRENT_OS} STREQUAL "ios")
    file(GLOB OBJC_HEADER_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/sink/objc/*.h")
    list(APPEND EXPORT_HEADER     ${OBJC_HEADER_FILE})
    file(GLOB OBJC_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/sink/objc/*")
    list(APPEND SOURCE_FILES ${OBJC_SOURCE_FILE})
elseif(${CURRENT_OS} STREQUAL "android")
    file(GLOB OBJC_HEADER_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/android/jni/*.h")
    list(APPEND EXPORT_HEADER     ${OBJC_HEADER_FILE})
    file(GLOB JNI_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/android/jni/*")
    list(APPEND SOURCE_FILES ${JNI_SOURCE_FILE})
endif()

# Library Configuration
# ----------------------------------------
# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${RESOURCE_FILES})

# Platform specific library configuration
if(${CURRENT_OS} STREQUAL "linux")
    # Configure Linux specific settings
    add_library(mars-face-kit SHARED IMPORTED)
    set_target_properties(mars-face-kit PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/libmars-face-kit.so)

    # Copy resources and libraries
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            ${CMAKE_CURRENT_SOURCE_DIR}/assets/res ${OUTPUT_INSTALL_PATH}/res
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            ${CMAKE_CURRENT_SOURCE_DIR}/assets/models ${OUTPUT_INSTALL_PATH}/models
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy 
            ${EXPORT_HEADER} ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy 
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/libmars-face-kit.so" 
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        COMMENT "Copying res and lib files to output/app directory.")

elseif(${CURRENT_OS} STREQUAL "windows")
    # Configure Windows specific settings
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")

    # Copy resources and libraries
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD 
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            ${CMAKE_CURRENT_SOURCE_DIR}/assets/res ${OUTPUT_INSTALL_PATH}/res
        COMMAND ${CMAKE_COMMAND} -E copy_directory 
            ${CMAKE_CURRENT_SOURCE_DIR}/assets/models ${OUTPUT_INSTALL_PATH}/models
        COMMAND ${CMAKE_COMMAND} -E make_directory 
            ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy 
            ${EXPORT_HEADER} ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
        COMMAND ${CMAKE_COMMAND} -E copy 
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/msvc-x64/mars-face-kit.dll" 
            ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
        COMMENT "Copying res and lib files to output/app directory.")

elseif(${CURRENT_OS} STREQUAL "macos" OR ${CURRENT_OS} STREQUAL "ios")
    # Configure macOS/iOS framework properties
    set_target_properties(${PROJECT_NAME} PROPERTIES
        XCODE_ATTRIBUTE_PRODUCT_NAME ${PROJECT_NAME}
        COMPILE_FLAGS "-x objective-c++"
        FRAMEWORK TRUE
        MACOSX_FRAMEWORK_IDENTIFIER net.pixpark.${PROJECT_NAME}
        PRODUCT_BUNDLE_IDENTIFIER net.pixpark.${PROJECT_NAME}
        CMAKE_XCODE_ATTRIBUTE_BUILT_PRODUCTS_DIR ${PROJECT_NAME}
        MACOSX_FRAMEWORK_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
        FRAMEWORK_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/
        PUBLIC_HEADER "${EXPORT_HEADER}"
        RESOURCE "${RESOURCE_FILES}"
    )

elseif(${CURRENT_OS} STREQUAL "android")
    # Configure Android specific settings
    add_library(mars-face-kit SHARED IMPORTED)
    set_target_properties(mars-face-kit PROPERTIES IMPORTED_LOCATION
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/${ANDROID_ABI}/libmars-face-kit.so)
endif()

# Library Dependencies
# ----------------------------------------
if(${CURRENT_OS} STREQUAL "linux")
    target_link_libraries(${PROJECT_NAME} PRIVATE
        GL
        glfw
        mars-face-kit)
elseif(${CURRENT_OS} STREQUAL "windows")
    target_link_libraries(${PROJECT_NAME} 
        opengl32
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/libs/msvc-x64/glfw3.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/msvc-x64/mars-face-kit.lib)
elseif(${CURRENT_OS} STREQUAL "macos")
    target_link_libraries(${PROJECT_NAME}
        "-framework OpenGL"
        "-framework AppKit"
        "-framework QuartzCore"
        "-framework CoreVideo"
        "-framework CoreGraphics"
        "-framework AVFoundation"
        "-framework CoreMedia"
        "-framework Metal"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mnn/libs/${CURRENT_OS}/MNN.framework"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/libmars-face-kit.a"
    )
elseif(${CURRENT_OS} STREQUAL "ios")
    target_link_libraries(${PROJECT_NAME}
        "-framework OpenGLES"     
        "-framework UIKit"     
        "-framework QuartzCore"  
        "-framework CoreVideo"  
        "-framework CoreGraphics"
        "-framework AVFoundation"
        "-framework CoreMedia"
        "-framework Metal"
        "-framework CoreML"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mnn/libs/${CURRENT_OS}/MNN.framework"
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/libmars-face-kit.a"
    )
elseif(${CURRENT_OS} STREQUAL "android")
    target_link_libraries(${PROJECT_NAME}
        log
        android
        GLESv3
        EGL
        jnigraphics
        mars-face-kit)
endif()