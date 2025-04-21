cmake_minimum_required(VERSION 3.16)

# ---- Project ----
# GPUPixel is a GPU-accelerated image processing library Supporting multiple
# platforms including desktop and mobile devices

# Note: update this to your new project's name and version
project(
  GPUPixel
  VERSION 1.3.0
  LANGUAGES CXX C)

# ---- Build options ----
# Configure how the library is built and packaged

option(GPUPIXEL_BUILD_SHARED_LIBS "Default build shared libraries" ON)
if(APPLE)
  option(GPUPIXEL_AAPL_FMWK
         "Build gpupixel.framework instead of traditional .a/.dylib" ON)
endif()

# face detection option
option(GPUPIXEL_ENABLE_FACE_DETECTOR "Enable face detection functionality" ON)

option(GPUPIXEL_BUILD_DESKTOP_DEMO "Build desktop demo" OFF)

if(GPUPIXEL_ENABLE_FACE_DETECTOR)
  add_compile_definitions(GPUPIXEL_ENABLE_FACE_DETECTOR)
endif()

# ---- Platform detection ----
# Identify the current operating system and set platform-specific variables
set(PLAT_LINUX "linux")
set(PLAT_WIN "windows")
set(PLAT_MAC "macos")
set(PLAT_IOS "ios")
set(PLAT_ANDROID "android")

set(CURRENT_PLAT "linux")
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(CURRENT_PLAT "linux")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(CURRENT_PLAT "windows")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CURRENT_PLAT "macos")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
  set(CURRENT_PLAT "ios")
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
  set(CURRENT_PLAT "android")
else()
  message(FATAL_ERROR "Unsupported operating system: ${CMAKE_SYSTEM_NAME}")
endif()

# ---- C++ standard settings ----
# Configure the C++ language version and features
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Print build options ----
message(STATUS "========================================")
message(STATUS "             BUILD OPTIONS              ")
message(STATUS "========================================")
message(STATUS "GPUPIXEL_BUILD_SHARED_LIBS: ${GPUPIXEL_BUILD_SHARED_LIBS}")
if(APPLE)
  message(STATUS "GPUPIXEL_AAPL_FMWK: ${GPUPIXEL_AAPL_FMWK}")
endif()
message(
  STATUS "GPUPIXEL_ENABLE_FACE_DETECTOR: ${GPUPIXEL_ENABLE_FACE_DETECTOR}")
message(STATUS "GPUPIXEL_BUILD_DESKTOP_DEMO: ${GPUPIXEL_BUILD_DESKTOP_DEMO}")

# ---- System information ----
message(STATUS "========================================")
message(STATUS "           SYSTEM INFORMATION           ")
message(STATUS "========================================")
message(STATUS "CMake Version: ${CMAKE_VERSION}")
message(STATUS "Operating System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Processor Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "C Compiler: ${CMAKE_C_COMPILER_ID} ${CMAKE_C_COMPILER_VERSION}")
message(
  STATUS "C++ Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")

# ---- Build configuration ----
message(STATUS "========================================")
message(STATUS "           BUILD CONFIGURATION          ")
message(STATUS "========================================")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "========================================")
message(STATUS "         CONFIGURATION COMPLETE         ")
message(STATUS "========================================")

# ---- Build safety checks ----
# Prevent in-source builds which can cause numerous issues
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(
    FATAL_ERROR
      "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there."
  )
endif()

# ---- Add dependencies via CPM ----
# see https://github.com/TheLartians/CPM.cmake for more info CPM is a CMake
# package manager that simplifies adding dependencies

# include(cmake/CPM.cmake)

# ---- Add source files ----
# External dependencies first
add_subdirectory(third_party)

# Main library source code
add_subdirectory(src)

# Optional desktop demo application
if(GPUPIXEL_BUILD_DESKTOP_DEMO)
  add_subdirectory(demo)
endif()
