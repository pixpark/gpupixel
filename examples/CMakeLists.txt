# 设置 cmkae 最低版本
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

# 设置引入的 cmake 文件路径为当前目录
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

# 定义当前模块的名称
set(APP_PROJECT_NAME "gpupixel_app")
project(${APP_PROJECT_NAME})

# Detect platform
# ------
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
# ------
set(OUTPUT_INSTALL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../output")
set(CMAKE_INCLUDE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/include")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/library/${CURRENT_OS}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_INSTALL_PATH}/app/${CURRENT_OS}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


link_directories(${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(APP_RESOURCE_DIR "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res/")
set(COPY_DST_RUNTIME_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(MODELS_DST_DIR ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/models)

# Config source and header file
# -------
# header include path
include_directories(
    ${CMAKE_INCLUDE_OUTPUT_DIRECTORY}
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glfw/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/stb
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glad/include
)

# Add common source file
file(GLOB SOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/desktop/*"
    )


# Add platform source and header and lib link search path
if(${CURRENT_OS} STREQUAL "windows")                                                         # windows
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
    # link libs find path
    link_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glfw/lib-mingw-w64)

    # Source
    file(GLOB GLAD_SOURCE_FILE "${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glad/src/*.c")
    list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})
elseif(${CURRENT_OS} STREQUAL "linux")
    # # Source
    # file(GLOB GLAD_SOURCE_FILE  "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/src/*.c" )
    # list(APPEND SOURCE_FILES ${GLAD_SOURCE_FILE})
endif()

file(GLOB MODULE_FILES
${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/mars-face-kit/models/*
)
# build type: executable
# ------
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# link libs
# -------
if(${CURRENT_OS} STREQUAL "linux")
    target_link_libraries(${PROJECT_NAME}
                        gpupixel
                        GL
                        glfw)
    set(GPUPIXEL_LIBS ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/libgpupixel.so)
    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./")
elseif(${CURRENT_OS} STREQUAL "windows")

    set(GPUPIXEL_LIBS     ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/gpupixel.dll
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/mars-face-kit.dll)

    target_link_libraries(${PROJECT_NAME}
                        ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}/gpupixel.lib
                        opengl32
                        ${CMAKE_CURRENT_SOURCE_DIR}/../src/third_party/glfw/libs/msvc-x64/glfw3.lib)

    set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-Wl,-rpath,./")
endif()

# copy resource file
# --------
# Add resource file
file(GLOB RESOURCE_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../src/resources/*"
)


macro(EXPORT_INCLUDE)
add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD

                COMMAND ${CMAKE_COMMAND} -E make_directory ${APP_RESOURCE_DIR}

                COMMAND ${CMAKE_COMMAND} -E copy
                ${RESOURCE_FILES} ${APP_RESOURCE_DIR}

                COMMENT "Copying resource files to output/app directory.")


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory ${MODELS_DST_DIR}

                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${MODULE_FILES} ${MODELS_DST_DIR}

                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${CMAKE_CURRENT_SOURCE_DIR}/../examples/desktop/demo.png" ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}

                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${GPUPIXEL_LIBS} ${COPY_DST_RUNTIME_DIR}
                )

endmacro()
EXPORT_INCLUDE()
