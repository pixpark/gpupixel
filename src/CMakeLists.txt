# 定义项目名称并初始化项目
set(gpupixel_libs_name "gpupixel")

# index file
set(common_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_program.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_context.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_framebuffer.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_framebuffer_factory.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/source/source.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/source/source_raw_data.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/source/source_image.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sink/sink_raw_data.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sink/sink_render.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/sink/sink.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/math_toolbox.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/dispatch_queue.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/util.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/contrast_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/glass_sphere_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/brightness_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/ios_blur_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/hsb_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/sobel_edge_detection_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/sphere_refraction_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/directional_sobel_edge_detection_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/blusher_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/box_high_pass_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/luminance_range_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/box_blur_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/sketch_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/directional_non_maximum_suppression_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/toon_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/pixellation_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/beauty_face_unit_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/single_component_gaussian_blur_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/non_maximum_suppression_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/canny_edge_detection_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/bilateral_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/color_matrix_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/exposure_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/rgb_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/face_makeup_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/hue_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/nearby_sampling3x3_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/posterize_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/color_invert_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/single_component_gaussian_blur_mono_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/gaussian_blur_mono_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/convolution3x3_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/weak_pixel_inclusion_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/halftone_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/saturation_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/emboss_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/grayscale_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/lipstick_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/box_mono_blur_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/box_difference_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/crosshatch_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/filter_group.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/gaussian_blur_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/beauty_face_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/face_reshape_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/white_balance_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/filter/smooth_toon_filter.cc)

# 根据选项添加人脸检测源文件
if(GPUPIXEL_ENABLE_FACE_DETECTOR)
  list(APPEND common_source_files
       ${CMAKE_CURRENT_SOURCE_DIR}/face_detector/face_detector.cc)
endif()

set(objc_source_files ${CMAKE_CURRENT_SOURCE_DIR}/sink/sink_view.cc
                      ${CMAKE_CURRENT_SOURCE_DIR}/sink/objc_view.mm)

set(jni_source_files
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_gpupixel.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_filter.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_source_raw_data.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_source.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_helpers.h
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_helpers.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_source_image.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_face_detector.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_sink_raw_data.cc)

set(lib_source_code_files ${common_source_files})

if(${CURRENT_PLAT} STREQUAL ${PLAT_ANDROID})
  list(APPEND lib_source_code_files ${jni_source_files})
elseif(${CURRENT_PLAT} STREQUAL ${PLAT_MAC} OR ${CURRENT_PLAT} STREQUAL
                                               ${PLAT_IOS})
  list(APPEND lib_source_code_files ${objc_source_files})
endif()

# header files
set(public_common_header_files
    ${PROJECT_SOURCE_DIR}/include/gpupixel/gpupixel.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/gpupixel_define.h)

# 根据选项添加人脸检测头文件
if(GPUPIXEL_ENABLE_FACE_DETECTOR)
  set(public_face_detector_header_files
      ${PROJECT_SOURCE_DIR}/include/gpupixel/face_detector/face_detector.h)
else()
  set(public_face_detector_header_files "")
endif()

set(public_source_header_files
    ${PROJECT_SOURCE_DIR}/include/gpupixel/source/source.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/source/source_raw_data.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/source/source_image.h)

set(public_sink_header_files
    ${PROJECT_SOURCE_DIR}/include/gpupixel/sink/sink_raw_data.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/sink/sink.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/sink/sink_render.h)

set(public_objc_sink_header_files
    ${PROJECT_SOURCE_DIR}/include/gpupixel/sink/sink_view.h)

set(public_utils_header_files
    ${PROJECT_SOURCE_DIR}/include/gpupixel/utils/math_toolbox.h)

set(public_filter_header_files
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/gaussian_blur_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/non_maximum_suppression_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/weak_pixel_inclusion_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/crosshatch_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/box_high_pass_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/rgb_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/white_balance_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/smooth_toon_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/brightness_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/single_component_gaussian_blur_mono_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/grayscale_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/toon_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/contrast_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/lipstick_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/directional_non_maximum_suppression_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/glass_sphere_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/sketch_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/convolution3x3_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/luminance_range_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/posterize_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/exposure_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/sphere_refraction_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/color_matrix_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/hue_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/blusher_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/bilateral_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/face_reshape_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/box_mono_blur_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/nearby_sampling3x3_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/gaussian_blur_mono_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/face_makeup_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/emboss_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/canny_edge_detection_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/box_difference_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/beauty_face_unit_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/hsb_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/directional_sobel_edge_detection_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/sobel_edge_detection_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/beauty_face_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/single_component_gaussian_blur_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/box_blur_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/saturation_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/color_invert_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/filter_group.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/ios_blur_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/halftone_filter.h
    ${PROJECT_SOURCE_DIR}/include/gpupixel/filter/pixellation_filter.h)

set(internal_core_header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_program.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_framebuffer_factory.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_context.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_framebuffer.h
    ${CMAKE_CURRENT_SOURCE_DIR}/core/gpupixel_gl_include.h)

set(internal_objc_sink_header_files ${PROJECT_SOURCE_DIR}/src/sink/objc_view.h)

set(internal_utils_header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/dispatch_queue.h
    ${CMAKE_CURRENT_SOURCE_DIR}/utils/util.h)

set(internal_jni_header_files
    ${CMAKE_CURRENT_SOURCE_DIR}/android/jni/jni_helpers.h)

set(lib_header_code_files
    ${public_common_header_files}
    ${public_face_detector_header_files}
    ${public_source_header_files}
    ${public_sink_header_files}
    ${public_filter_header_files}
    ${public_utils_header_files}
    ${internal_core_header_files}
    ${internal_objc_sink_header_files}
    ${internal_utils_header_files})

if(${CURRENT_PLAT} STREQUAL ${PLAT_ANDROID})
  list(APPEND lib_header_code_files ${internal_jni_header_files})
elseif(${CURRENT_PLAT} STREQUAL ${PLAT_MAC} OR ${CURRENT_PLAT} STREQUAL
                                               ${PLAT_IOS})
  list(APPEND lib_header_code_files ${public_objc_sink_header_files})
endif()

set(lib_image_resource_files
    ${CMAKE_CURRENT_SOURCE_DIR}/res/mouth.png
    ${CMAKE_CURRENT_SOURCE_DIR}/res/blusher.png
    ${CMAKE_CURRENT_SOURCE_DIR}/res/lookup_gray.png
    ${CMAKE_CURRENT_SOURCE_DIR}/res/lookup_light.png
    ${CMAKE_CURRENT_SOURCE_DIR}/res/lookup_custom.png
    ${CMAKE_CURRENT_SOURCE_DIR}/res/lookup_origin.png
    ${CMAKE_CURRENT_SOURCE_DIR}/res/lookup_skin.png)

set(lib_models_resource_files
    ${PROJECT_SOURCE_DIR}/third_party/mars-face-kit/models/face_align.mars_model
    ${PROJECT_SOURCE_DIR}/third_party/mars-face-kit/models/face_det.mars_model)

if(GPUPIXEL_AAPL_FMWK)
  foreach(hdr ${public_common_header_files})
    set_source_files_properties(${hdr} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                  Headers)
  endforeach()

  foreach(hdr ${public_face_detector_header_files})
    set_source_files_properties(${hdr} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                  Headers/face_detector)
  endforeach()

  foreach(hdr ${public_source_header_files})
    set_source_files_properties(${hdr} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                  Headers/source)
  endforeach()

  foreach(hdr ${public_sink_header_files})
    set_source_files_properties(${hdr} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                  Headers/sink)
  endforeach()

  foreach(hdr ${public_objc_sink_header_files})
    set_source_files_properties(${hdr} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                  Headers/sink)
  endforeach()

  foreach(hdr ${public_filter_header_files})
    set_source_files_properties(${hdr} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                  Headers/filter)
  endforeach()

  foreach(hdr ${public_utils_header_files})
    set_source_files_properties(${hdr} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                  Headers/utils)
  endforeach()

  foreach(image ${lib_image_resource_files})
    set_source_files_properties(${image} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                    Resources/res)
  endforeach()

  foreach(model ${lib_models_resource_files})
    set_source_files_properties(${model} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                    Resources/models)
  endforeach()
endif()

# 库配置
# ##############################################################################

if(GPUPIXEL_BUILD_SHARED_LIBS)
  add_library(
    ${gpupixel_libs_name} SHARED
    ${lib_source_code_files} ${lib_header_code_files}
    ${lib_image_resource_files} ${lib_models_resource_files})
else()
  add_library(
    ${gpupixel_libs_name} STATIC
    ${lib_source_code_files} ${lib_header_code_files}
    ${lib_image_resource_files} ${lib_models_resource_files})
endif()

target_include_directories(
  ${gpupixel_libs_name} PUBLIC ${PROJECT_SOURCE_DIR}/include
                               ${CMAKE_CURRENT_SOURCE_DIR})

if(WIN32 AND GPUPIXEL_BUILD_SHARED_LIBS)
  # Windows动态库导出宏定义
  target_compile_definitions(${gpupixel_libs_name}
                             PRIVATE -DBUILDING_GPUPIXEL_DLL)

elseif(NOT WIN32)
  # 非Windows平台使用编译器可见性属性
  set(CMAKE_C_VISIBILITY_PRESET hidden)
  set(CMAKE_CXX_VISIBILITY_PRESET hidden)
  set(CMAKE_VISIBILITY_INLINES_HIDDEN ON)

  # Release模式强制隐藏符号
  if(NOT CMAKE_BUILD_TYPE MATCHES Debug)
    target_compile_options(
      ${gpupixel_libs_name} PRIVATE -fvisibility=hidden
                                    -fvisibility-inlines-hidden)
  endif()

  # 保留Objective-C符号可见性
  set_target_properties(
    ${gpupixel_libs_name} PROPERTIES OBJCXX_VISIBILITY_PRESET default
                                     OBJC_VISIBILITY_PRESET default)
endif()

if(WIN32)
  # Windows字符集配置
  add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
  add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")

  # being a cross-platform target, we enforce standards conformance on MSVC
  target_compile_options(
    ${gpupixel_libs_name}
    PUBLIC "$<$<COMPILE_LANG_AND_ID:CXX,MSVC>:/permissive->")
endif()

if(APPLE)
  set_target_properties(${gpupixel_libs_name} PROPERTIES COMPILE_FLAGS
                                                         "-x objective-c++")

  if(GPUPIXEL_AAPL_FMWK)
    set(APPLE_FRAMEWORK_NAME ${gpupixel_libs_name})
    set(APPLE_FMWK_VERSION "1.0.0")
    set(APPLE_FMWK_SOVERSION "1.0")
    set_target_properties(
      ${gpupixel_libs_name}
      PROPERTIES XCODE_ATTRIBUTE_PRODUCT_NAME ${gpupixel_libs_name}
                 FRAMEWORK TRUE
                 FRAMEWORK_VERSION A
                 MACOSX_FRAMEWORK_SHORT_VERSION_STRING ${APPLE_FMWK_SOVERSION}
                 VERSION ${APPLE_FMWK_VERSION}
                 SOVERSION ${APPLE_FMWK_SOVERSION}
                 MACOSX_FRAMEWORK_IDENTIFIER "net.pixpark.${gpupixel_libs_name}"
                 MACOSX_FRAMEWORK_INFO_PLIST
                 "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
                 XCODE_ATTRIBUTE_GCC_SYMBOLS_PRIVATE_EXTERN NO)

    if(${CURRENT_PLAT} STREQUAL ${PLAT_IOS})
      set(APPLE_DEPLOYMENT_TARGET "11.0")
      set_target_properties(
        ${gpupixel_libs_name} PROPERTIES IPHONEOS_DEPLOYMENT_TARGET
                                         ${APPLE_DEPLOYMENT_TARGET})
    endif()

    if(${CURRENT_PLAT} STREQUAL ${PLAT_MAC})
      set(APPLE_DEPLOYMENT_TARGET "10.15")
      set_target_properties(
        ${gpupixel_libs_name} PROPERTIES MACOSX_DEPLOYMENT_TARGET
                                         ${APPLE_DEPLOYMENT_TARGET})
    endif()
  endif()
endif()

# 链接库依赖
# ##############################################################################
# Linux平台依赖
if(${CURRENT_PLAT} STREQUAL ${PLAT_LINUX})
  target_link_libraries(${gpupixel_libs_name} PRIVATE GL marsface yuv glad glfw
                                                      stb)

  # Windows平台依赖
elseif(${CURRENT_PLAT} STREQUAL ${PLAT_WIN})
  target_link_libraries(
    ${gpupixel_libs_name}
    PRIVATE opengl32
            glfw
            marsface
            yuv
            glad
            glfw
            stb)

  # macOS平台依赖
elseif(${CURRENT_PLAT} STREQUAL ${PLAT_MAC})
  target_link_libraries(
    ${gpupixel_libs_name}
    PRIVATE "-framework OpenGL"
            "-framework AppKit"
            "-framework QuartzCore"
            "-framework CoreVideo"
            "-framework CoreGraphics"
            "-framework AVFoundation"
            "-framework CoreMedia"
            "-framework Metal"
            mnn
            marsface
            yuv
            stb)

  # iOS平台依赖
elseif(${CURRENT_PLAT} STREQUAL ${PLAT_IOS})
  target_link_libraries(
    ${gpupixel_libs_name}
    PRIVATE "-framework Foundation"
            "-framework OpenGLES"
            "-framework UIKit"
            "-framework QuartzCore"
            "-framework CoreVideo"
            "-framework CoreGraphics"
            "-framework AVFoundation"
            "-framework CoreMedia"
            "-framework Metal"
            "-framework CoreML"
            mnn
            marsface
            yuv
            stb)

  # Android平台依赖
elseif(${CURRENT_PLAT} STREQUAL ${PLAT_ANDROID})
  target_link_libraries(
    ${gpupixel_libs_name}
    PRIVATE log
            android
            GLESv3
            EGL
            jnigraphics
            marsface
            yuv
            stb)
endif()

# install header

install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ DESTINATION include)

if(GPUPIXEL_AAPL_FMWK)
  foreach(hdr ${filter_header_files})
    set_source_files_properties(${hdr} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                  Headers/filter)
  endforeach()

  foreach(image ${lib_image_resource_files})
    set_source_files_properties(${image} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                    Resources/res)
  endforeach()

  foreach(model ${lib_models_resource_files})
    set_source_files_properties(${model} PROPERTIES MACOSX_PACKAGE_LOCATION
                                                    Resources/models)
  endforeach()

  # add ad-hoc signing for framework
  add_custom_command(
    TARGET ${gpupixel_libs_name}
    POST_BUILD
    COMMAND codesign --force --sign - "$<TARGET_FILE_DIR:${gpupixel_libs_name}>"
    COMMENT
      "Ad-Hoc signing with deep option completed for ${gpupixel_libs_name}.framework"
  )
endif()

# install library
install(
  TARGETS ${gpupixel_libs_name}
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION lib # win dll install lib
  FRAMEWORK DESTINATION lib)

# install image resource
install(
  FILES ${lib_image_resource_files}
  DESTINATION res
  COMPONENT resource)

# install model resource
install(
  FILES ${lib_models_resource_files}
  DESTINATION models
  COMPONENT resource)
