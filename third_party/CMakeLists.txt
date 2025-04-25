# ---- Mars face library configuration ----
# Configure the face detection and tracking library for all platforms
if(GPUPIXEL_ENABLE_FACE_DETECTOR)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    # Android platform: dynamic library .so
    add_library(marsface SHARED IMPORTED GLOBAL)
    set(mars_shared_lib_path
        ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/android/${ANDROID_ABI}/libmars-face-kit.so
    )

  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows platform: requires .dll (runtime) + .lib (link time)
    add_library(marsface SHARED IMPORTED GLOBAL)
    set(mars_shared_lib_path
        ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/windows/msvc-x64/mars-face-kit.dll
    )
    set(mars_import_lib_path
        ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/windows/msvc-x64/mars-face-kit.lib
    )

  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux platform: dynamic library .so
    add_library(marsface SHARED IMPORTED GLOBAL)
    set(mars_shared_lib_path
        ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/linux/libmars-face-kit.so
    )

  elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # macOS platform: static library .a
    add_library(marsface STATIC IMPORTED GLOBAL)
    set(mars_shared_lib_path
        ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/macos/libmars-face-kit.a)
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    # iOS platform: static library .a
    add_library(marsface STATIC IMPORTED GLOBAL)
    set(mars_shared_lib_path
        ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/ios/libmars-face-kit.a)
  endif()

  if(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    add_library(marsface::marsface ALIAS marsface)
  endif()

  # set header path (common for all platforms)
  set(mars_face_header ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/include)

  # ---- Library import properties ----
  # Configure how imported libraries are found and used
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    # Windows special handling: specify both .lib (IMPORTED_IMPLIB) and .dll
    # (IMPORTED_LOCATION)
    set_target_properties(
      marsface
      PROPERTIES IMPORTED_IMPLIB ${mars_import_lib_path} # .lib file for linking
                 IMPORTED_LOCATION ${mars_shared_lib_path} # .dll file for
                                                           # runtime
                 INTERFACE_INCLUDE_DIRECTORIES ${mars_face_header})
  elseif(NOT ${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set_target_properties(
      marsface PROPERTIES IMPORTED_LOCATION ${mars_shared_lib_path}
                          INTERFACE_INCLUDE_DIRECTORIES ${mars_face_header})
  endif()

  # ---- Runtime library installation ----
  # Install shared libraries that are needed at runtime
  if(${CMAKE_SYSTEM_NAME} MATCHES "Windows" OR ${CMAKE_SYSTEM_NAME} MATCHES
                                               "Linux")
    install(
      FILES ${mars_shared_lib_path}
      DESTINATION lib
      COMPONENT runtime)
  endif()

  # ---- MNN library configuration ----
  # Neural network acceleration framework (Apple platforms only)
  if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    add_library(mnn STATIC IMPORTED GLOBAL)
    set(mnn_lib_path ${CMAKE_CURRENT_SOURCE_DIR}/mnn/libs/macos)

    # find library file
    find_library(
      mnn_lib
      NAMES "MNN"
      PATHS ${mnn_lib_path}
      NO_DEFAULT_PATH)

    set_target_properties(mnn PROPERTIES IMPORTED_LOCATION ${mnn_lib})
  elseif(${CMAKE_SYSTEM_NAME} MATCHES "iOS")
    add_library(mnn STATIC IMPORTED GLOBAL)
    set(mnn_lib_path ${CMAKE_CURRENT_SOURCE_DIR}/mnn/libs/ios)

    # find library file
    find_library(
      mnn_lib
      NAMES "MNN"
      PATHS ${mnn_lib_path}
      NO_DEFAULT_PATH)

    set_target_properties(mnn PROPERTIES IMPORTED_LOCATION ${mnn_lib})
  endif()
endif()
# ---- libyuv configuration ----
# YUV image processing library for efficient color space conversion
add_subdirectory(libyuv EXCLUDE_FROM_ALL) # EXCLUDE_FROM_ALL means don't install
                                          # yuv target

add_library(libyuv::yuv ALIAS yuv)

target_include_directories(
  yuv PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libyuv/include>)

# add -fPIC option to solve dynamic library linking issues
set_target_properties(yuv PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  install(
    TARGETS yuv
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
endif()

# ---- Desktop platform dependencies ----
# Libraries specific to desktop platforms (macOS, Windows, Linux)
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin"
   OR ${CMAKE_SYSTEM_NAME} MATCHES "Windows"
   OR ${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # ---- glad configuration ----
  # OpenGL loader library
  add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c)

  add_library(glad::glad ALIAS glad)

  # add -fPIC option to solve dynamic library linking issues
  set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

  target_include_directories(
    glad PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad/include>)

  # ---- glfw configuration ----
  # Cross-platform window and input management
  set(GLFW_BUILD_EXAMPLES
      OFF
      CACHE BOOL "Disable building GLFW examples")
  set(GLFW_BUILD_TESTS
      OFF
      CACHE BOOL "Disable building GLFW tests")
  set(GLFW_INSTALL
      OFF
      CACHE BOOL "Disable GLFW installation")
  add_subdirectory(glfw EXCLUDE_FROM_ALL)

  add_library(glfw::glfw ALIAS glfw)

  if(APPLE)
    # disable ARC for Objective-C and Objective-C++ files
    target_compile_options(glfw PRIVATE "-fno-objc-arc")
  endif()
endif()

# ---- stb image library configuration ----
# Header-only image loading/saving library
add_library(stb INTERFACE)

target_include_directories(
  stb INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb/include>)

add_library(stb::stb ALIAS stb)

# ---- ghc_filesystem configuration ----
# Header-only filesystem library
add_library(ghc_filesystem INTERFACE)

add_library(ghc::filesystem ALIAS ghc_filesystem)

target_include_directories(
  ghc_filesystem
  INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/ghc/include>)
