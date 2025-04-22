# ---- Mars face library configuration ----
# Configure the face detection and tracking library for all platforms
if(${CURRENT_PLAT} STREQUAL ${PLAT_ANDROID})
  # Android platform: dynamic library .so
  add_library(marsface SHARED IMPORTED GLOBAL)
  set(mars_shared_lib_path
      ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/${CURRENT_PLAT}/${ANDROID_ABI}/libmars-face-kit.so
  )

elseif(${CURRENT_PLAT} STREQUAL ${PLAT_WIN})
  # Windows platform: requires .dll (runtime) + .lib (link time)
  add_library(marsface SHARED IMPORTED GLOBAL)
  set(mars_shared_lib_path
      ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/${CURRENT_PLAT}/msvc-x64/mars-face-kit.dll
  )
  set(mars_import_lib_path
      ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/${CURRENT_PLAT}/msvc-x64/mars-face-kit.lib
  )

elseif(${CURRENT_PLAT} STREQUAL ${PLAT_LINUX})
  # Linux platform: dynamic library .so
  add_library(marsface SHARED IMPORTED GLOBAL)
  set(mars_shared_lib_path
      ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/${CURRENT_PLAT}/libmars-face-kit.so
  )

elseif(${CURRENT_PLAT} STREQUAL ${PLAT_MAC} OR ${CURRENT_PLAT} STREQUAL
                                               ${PLAT_IOS})
  # macOS/iOS platform: static library .a
  add_library(marsface STATIC IMPORTED GLOBAL)
  set(mars_shared_lib_path
      ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/libs/${CURRENT_PLAT}/libmars-face-kit.a
  )
endif()

# set header path (common for all platforms)
set(mars_face_header ${CMAKE_CURRENT_SOURCE_DIR}/mars-face-kit/include)

# ---- Library import properties ----
# Configure how imported libraries are found and used
if(${CURRENT_PLAT} STREQUAL ${PLAT_WIN})
  # Windows special handling: specify both .lib (IMPORTED_IMPLIB) and .dll
  # (IMPORTED_LOCATION)
  set_target_properties(
    marsface
    PROPERTIES IMPORTED_IMPLIB ${mars_import_lib_path} # .lib file for linking
               IMPORTED_LOCATION ${mars_shared_lib_path} # .dll file for runtime
               INTERFACE_INCLUDE_DIRECTORIES ${mars_face_header})
elseif(NOT ${CURRENT_PLAT} STREQUAL ${PLAT_WASM})
  set_target_properties(
    marsface PROPERTIES IMPORTED_LOCATION ${mars_shared_lib_path}
                        INTERFACE_INCLUDE_DIRECTORIES ${mars_face_header})
endif()

# ---- Runtime library installation ----
# Install shared libraries that are needed at runtime
if(${CURRENT_PLAT} STREQUAL ${PLAT_WIN} OR ${CURRENT_PLAT} STREQUAL
                                           ${PLAT_LINUX})
  install(
    FILES ${mars_shared_lib_path}
    DESTINATION lib
    COMPONENT runtime)
endif()

# ---- MNN library configuration ----
# Neural network acceleration framework (Apple platforms only)
if(${CURRENT_PLAT} STREQUAL ${PLAT_MAC} OR ${CURRENT_PLAT} STREQUAL ${PLAT_IOS})
  add_library(mnn STATIC IMPORTED GLOBAL)
  set(mnn_lib_path ${CMAKE_CURRENT_SOURCE_DIR}/mnn/libs/${CURRENT_PLAT})

  # find library file
  find_library(
    mnn_lib
    NAMES "MNN"
    PATHS ${mnn_lib_path}
    NO_DEFAULT_PATH)

  set_target_properties(mnn PROPERTIES IMPORTED_LOCATION ${mnn_lib})
endif()

# ---- libyuv configuration ----
# YUV image processing library for efficient color space conversion
add_subdirectory(libyuv EXCLUDE_FROM_ALL) # EXCLUDE_FROM_ALL means don't install
                                          # yuv target

target_include_directories(
  yuv PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/libyuv/include>)

# add -fPIC option to solve dynamic library linking issues
set_target_properties(yuv PROPERTIES POSITION_INDEPENDENT_CODE ON)

if(${CURRENT_PLAT} STREQUAL ${PLAT_WASM})
  install(
    TARGETS yuv
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib)
endif()

# ---- Desktop platform dependencies ----
# Libraries specific to desktop platforms (macOS, Windows, Linux)
if(${CURRENT_PLAT} STREQUAL ${PLAT_MAC}
   OR ${CURRENT_PLAT} STREQUAL ${PLAT_WIN}
   OR ${CURRENT_PLAT} STREQUAL ${PLAT_LINUX})
  # ---- glad configuration ----
  # OpenGL loader library
  add_library(glad STATIC ${CMAKE_CURRENT_SOURCE_DIR}/glad/src/glad.c)

  # add -fPIC option to solve dynamic library linking issues
  set_target_properties(glad PROPERTIES POSITION_INDEPENDENT_CODE ON)

  target_include_directories(
    glad PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/glad/include>)

  # ---- glfw configuration ----
  # Cross-platform window and input management
  add_subdirectory(glfw EXCLUDE_FROM_ALL)

  if(APPLE)
    # disable ARC for Objective-C and Objective-C++ files
    target_compile_options(
      glfw PRIVATE $<$<COMPILE_LANGUAGE:OBJC>:-fno-objc-arc>
                   $<$<COMPILE_LANGUAGE:OBJCXX>:-fno-objc-arc>)
  endif()
endif()

# ---- stb image library configuration ----
# Header-only image loading/saving library
add_library(stb INTERFACE)

target_include_directories(
  stb INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/stb>)
