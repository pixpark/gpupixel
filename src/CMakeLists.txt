# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("gpupixel")

add_compile_definitions(GPUPIXEL_ANDROID)

# header search path
include_directories(
                    ./
                    ./filter
                    ./target
                    ./source
                    ./utils
                    ../third-party/libyuv/include)

# lib search path, support static and shared lib
#link_directories(
#        ../../../third_party/libs/ffmpeg/android/${ANDROID_ABI}
#)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        gpupixel

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        GPUPixelContext.cc
        DispatchQueue.cc
        Framebuffer.cc
        FramebufferCache.cc
        GLProgram.cc
        GPUPixelJNI.cc
        jni_helpers.cc
        MathToolbox.cc
        Util.cc
        filter/BaseBeautyFaceFilter.cc
        filter/FaceBeautyFilter.cc
        filter/BoxHighPassFilter.cc
        filter/BoxDifferenceFilter.cc
        filter/BoxBlurFilter.cc
        filter/BoxMonoBlurFilter.cc
        filter/BilateralFilter.cc
        filter/BrightnessFilter.cc
        filter/CannyEdgeDetectionFilter.cc
        filter/ColorInvertFilter.cc
        filter/ColorMatrixFilter.cc
        filter/ContrastFilter.cc
        filter/Convolution3x3Filter.cc
        filter/CrosshatchFilter.cc
        filter/DirectionalNonMaximumSuppressionFilter.cc
        filter/DirectionalSobelEdgeDetectionFilter.cc
        filter/EmbossFilter.cc
        filter/ExposureFilter.cc
        filter/Filter.cc
        filter/FilterGroup.cc
        filter/GaussianBlurFilter.cc
        filter/GaussianBlurMonoFilter.cc
        filter/GlassSphereFilter.cc
        filter/GrayscaleFilter.cc
        filter/HSBFilter.cc
        filter/HalftoneFilter.cc
        filter/HueFilter.cc
        filter/LuminanceRangeFilter.cc
        filter/NearbySampling3x3Filter.cc
        filter/NonMaximumSuppressionFilter.cc
        filter/PixellationFilter.cc
        filter/PosterizeFilter.cc
        filter/RGBFilter.cc
        filter/SaturationFilter.cc
        filter/SingleComponentGaussianBlurFilter.cc
        filter/SingleComponentGaussianBlurMonoFilter.cc
        filter/SketchFilter.cc
        filter/SmoothToonFilter.cc
        filter/SobelEdgeDetectionFilter.cc
        filter/SphereRefractionFilter.cc
        filter/ToonFilter.cc
        filter/WeakPixelInclusionFilter.cc
        filter/WhiteBalanceFilter.cc
        source/Source.cc
        source/SourceCamera.cc
        source/SourceImage.cc
        source/SourceRawDataInput.cc
        target/Target.cc
        target/TargetRawDataOutput.cc
        target/TargetView.cc
        ../third-party/libyuv/source/compare.cc           
        ../third-party/libyuv/source/compare_common.cc    
        ../third-party/libyuv/source/compare_gcc.cc       
        ../third-party/libyuv/source/compare_msa.cc       
        ../third-party/libyuv/source/compare_neon.cc      
        ../third-party/libyuv/source/compare_neon64.cc    
        ../third-party/libyuv/source/compare_win.cc       
        ../third-party/libyuv/source/convert.cc           
        ../third-party/libyuv/source/convert_argb.cc      
        ../third-party/libyuv/source/convert_from.cc      
        ../third-party/libyuv/source/convert_from_argb.cc 
        ../third-party/libyuv/source/convert_to_argb.cc   
        ../third-party/libyuv/source/convert_to_i420.cc   
        ../third-party/libyuv/source/cpu_id.cc            
        ../third-party/libyuv/source/planar_functions.cc  
        ../third-party/libyuv/source/rotate.cc            
        ../third-party/libyuv/source/rotate_any.cc        
        ../third-party/libyuv/source/rotate_argb.cc       
        ../third-party/libyuv/source/rotate_common.cc     
        ../third-party/libyuv/source/rotate_gcc.cc        
        ../third-party/libyuv/source/rotate_msa.cc        
        ../third-party/libyuv/source/rotate_neon.cc       
        ../third-party/libyuv/source/rotate_neon64.cc     
        ../third-party/libyuv/source/rotate_win.cc        
        ../third-party/libyuv/source/row_any.cc           
        ../third-party/libyuv/source/row_common.cc        
        ../third-party/libyuv/source/row_gcc.cc           
        ../third-party/libyuv/source/row_msa.cc           
        ../third-party/libyuv/source/row_neon.cc          
        ../third-party/libyuv/source/row_neon64.cc        
        ../third-party/libyuv/source/row_win.cc           
        ../third-party/libyuv/source/scale.cc             
        ../third-party/libyuv/source/scale_any.cc         
        ../third-party/libyuv/source/scale_argb.cc        
        ../third-party/libyuv/source/scale_common.cc      
        ../third-party/libyuv/source/scale_gcc.cc         
        ../third-party/libyuv/source/scale_msa.cc         
        ../third-party/libyuv/source/scale_neon.cc        
        ../third-party/libyuv/source/scale_neon64.cc      
        ../third-party/libyuv/source/scale_rgb.cc         
        ../third-party/libyuv/source/scale_uv.cc          
        ../third-party/libyuv/source/scale_win.cc         
        ../third-party/libyuv/source/video_common.cc
        )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        gpupixel

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        GLESv3
        EGL
        jnigraphics)