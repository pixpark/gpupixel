cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_COMPILE_WARNING_AS_ERROR ON)

# Set cmake module path to current directory
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# Define project name
set(PROJECT_NAME "gpupixel")
project(${PROJECT_NAME})

# Define API export macros (Valid for Windows platform)
add_definitions(-DMYMATH_EXPORT_LIBRARY)

# Detect platform
# --------
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

# Config build output path
# --------
set(OUTPUT_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output")
set(OUTPUT_RESOURCE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output/res")
set(OUTPUT_MODELS_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output/models")
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/include")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Config source and header file
# ---------
# header include path
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/core
    ${CMAKE_CURRENT_SOURCE_DIR}/filter
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_CURRENT_SOURCE_DIR}/target
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/face_detect
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni
    ${CMAKE_CURRENT_SOURCE_DIR}/target/objc
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/libyuv/include
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/include
)
 
# Add common source file
file(GLOB SOURCE_FILES     
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*"        
    "${CMAKE_CURRENT_SOURCE_DIR}/filter/*"         
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*"       
    "${CMAKE_CURRENT_SOURCE_DIR}/target/*"                               
    "${CMAKE_CURRENT_SOURCE_DIR}/face_detect/*"                 
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*"                 
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/libyuv/source/*"
)

# Add export header file
file(GLOB EXPORT_HEADER 
    "${CMAKE_CURRENT_SOURCE_DIR}/core/*.h"         
    "${CMAKE_CURRENT_SOURCE_DIR}/filter/*.h"         
    "${CMAKE_CURRENT_SOURCE_DIR}/source/*.h"       
    "${CMAKE_CURRENT_SOURCE_DIR}/target/*.h"                      
    "${CMAKE_CURRENT_SOURCE_DIR}/utils/*.h"                 
    "${CMAKE_CURRENT_SOURCE_DIR}/face_detect/*.h"                 
)

file(GLOB RESOURCE_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/resources/*"                      
)

file(GLOB MODEL_FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/models/*"                      
)

# Add platform source and header and lib link search path
if(${CURRENT_OS} STREQUAL "windows")     
    set(CMAKE_SHARED_LIBRARY_PREFIX "")                                                    # windows
    # Source 
    file(GLOB GLAD_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/*.c" )
    list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})

elseif(${CURRENT_OS} STREQUAL "linux")    
    # Source 
    file(GLOB GLAD_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/*.c" )
    list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})
elseif(${CURRENT_OS} STREQUAL "macos" OR ${CURRENT_OS} STREQUAL "ios")                        # ios and mac
    # Header
    file(GLOB OBJC_HEADER_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/target/objc/*.h")
    list(APPEND EXPORT_HEADER     ${OBJC_HEADER_FILE})
    
    # Source 
    file(GLOB OBJC_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/target/objc/*")
    list(APPEND SOURCE_FILES ${OBJC_SOURCE_FILE})

    
elseif(${CURRENT_OS} STREQUAL "android")                                                    # android 
    # Header
    file(GLOB OBJC_HEADER_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/android/jni/*.h")
    list(APPEND EXPORT_HEADER     ${OBJC_HEADER_FILE})
    
    # Source 
    file(GLOB JNI_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/android/jni/*")
    list(APPEND SOURCE_FILES ${JNI_SOURCE_FILE})

endif()

# Config project 
# ----------
# build shared or static lib
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${MODEL_FILES} ${RESOURCE_FILES})

# set platform project 
if(${CURRENT_OS} STREQUAL "linux")
    add_library(mars-face-kit SHARED IMPORTED)
    # Set actual path of target library
    set_target_properties(mars-face-kit PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/libmars-face-kit.so)

elseif(${CURRENT_OS} STREQUAL "windows")
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
 
elseif(${CURRENT_OS} STREQUAL "macos" OR ${CURRENT_OS} STREQUAL "ios")
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
    )

    # Add custom resource copy commands for macOS/iOS
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        # Copy model files to models directory
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/models"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        
        # Copy resource files to resources directory
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/resources"
            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
        
        COMMENT "Organizing resources in framework bundle"
    )
 
elseif(${CURRENT_OS} STREQUAL "android")
    add_library(mars-face-kit SHARED IMPORTED)
    # Set actual path of target library
    set_target_properties(mars-face-kit PROPERTIES IMPORTED_LOCATION
    ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/${ANDROID_ABI}/libmars-face-kit.so)
endif()
 
# link libs
# -------
if(${CURRENT_OS} STREQUAL "linux")
    target_link_libraries(
                            ${PROJECT_NAME}  
                            PRIVATE
                            GL
                            glfw
                            mars-face-kit)
elseif(${CURRENT_OS} STREQUAL "windows")
    target_link_libraries(
                        ${PROJECT_NAME} 
                        opengl32
                        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw/libs/msvc-x64/glfw3.lib
                        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/msvc-x64/mars-face-kit.lib)
elseif(${CURRENT_OS} STREQUAL "macos")
    target_link_libraries(
        ${PROJECT_NAME} "-framework OpenGL"
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
    target_link_libraries(
    ${PROJECT_NAME} "-framework OpenGLES"     
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
    target_link_libraries(
                    ${PROJECT_NAME}  
                    log
                    android
                    GLESv3
                    EGL
                    jnigraphics
                    mars-face-kit)
endif()


 

# copy DLL only on Windows
if(${CURRENT_OS} STREQUAL "windows")
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy 
        "${CMAKE_CURRENT_SOURCE_DIR}/third_party/mars-face-kit/libs/${CURRENT_OS}/msvc-x64/mars-face-kit.dll" ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}
    
    COMMENT "Copying resource files to output/app directory.")

endif()

# copy header
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD 
    # copy resource
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_RESOURCE_PATH}

    COMMAND ${CMAKE_COMMAND} -E copy 
    ${RESOURCE_FILES} ${OUTPUT_RESOURCE_PATH}
    
    # copy models
    COMMAND ${CMAKE_COMMAND} -E make_directory ${OUTPUT_MODELS_PATH}

    COMMAND ${CMAKE_COMMAND} -E copy 
    ${MODEL_FILES} ${OUTPUT_MODELS_PATH}

    # copy headers
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}

    COMMAND ${CMAKE_COMMAND} -E copy 
    ${EXPORT_HEADER} ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}

    COMMENT "Copying headers and resource to output directory.")