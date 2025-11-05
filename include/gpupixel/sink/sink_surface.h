/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

/**
 * @file sink_surface.h
 * @brief Android platform specific SinkSurface class header file
 *
 * This class is used to directly render GPU processed images to Android Surface, avoiding the overhead of copying data from C layer to Java layer.
 * Through EGL Window Surface, it renders directly in the C layer OpenGL environment, improving performance and reducing memory usage.
 *
 * Main features:
 * - Directly render processed images to Android Surface
 * - Support multiple fill modes (stretch, preserve aspect ratio, etc.)
 * - Support mirror rendering
 * - Automatically manage EGL Surface lifecycle
 */

#pragma once

#include "gpupixel/sink/sink.h"

#if defined(GPUPIXEL_ANDROID)
#include <android/native_window.h>
#include <EGL/egl.h>
#endif

namespace gpupixel {
    class GPUPixelGLProgram;

/**
 * @class SinkSurface
 * @brief Android platform specific rendering output class, directly renders to Android Surface
 *
 * Inherits from Sink class, implements the function of directly rendering GPU processed images to Android Surface.
 * Compared to `SinkRawData`, this class avoids data copying and has better performance.
 */
    class GPUPIXEL_API SinkSurface : public Sink {
    public:
        /**
         * @enum FillMode
         * @brief Image fill mode enumeration
         */
        enum FillMode {
            Stretch = 0,  // Stretch fill, may cause image distortion
            PreserveAspectRatio = 1,  // Preserve aspect ratio fill, may have black bars
            PreserveAspectRatioAndFill = 2  // Preserve aspect ratio and scale to fill, may crop some content
        };

#if defined(GPUPIXEL_ANDROID)
        /**
         * @brief Create SinkSurface instance
         * @return Smart pointer to SinkSurface
         */
        static std::shared_ptr<SinkSurface> Create();

        /**
         * @brief Destructor, automatically release resources
         */
        ~SinkSurface() override;

        /**
         * @brief Initialize rendering program
         * Create and compile default vertex and fragment shaders
         */
        void Init();

        /**
         * @brief Set input framebuffer
         * @param framebuffer Input framebuffer
         * @param rotation_mode Rotation mode
         * @param tex_idx Texture index
         */
        void SetInputFramebuffer(std::shared_ptr<GPUPixelFramebuffer> framebuffer,
                                 RotationMode rotation_mode = NoRotation,
                                 int tex_idx = 0) override;

        /**
         * @brief Set fill mode
         * @param fill_mode Fill mode (stretch, preserve aspect ratio, etc.)
         */
        void SetFillMode(FillMode fill_mode);

        /**
         * @brief Set mirror mode
         * @param mirror true for horizontal mirror, false for normal display
         */
        void SetMirror(bool mirror);

        /**
         * @brief Set Android native window
         * @param window ANativeWindow pointer (obtained from Surface)
         * @param width Window width
         * @param height Window height
         * This method creates EGL Window Surface and prepares for rendering
         */
        void SetNativeWindow(ANativeWindow* window, int width, int height);

        /**
         * @brief Release Android native window resources
         * Destroy EGL Surface and release ANativeWindow
         */
        void ReleaseNativeWindow();

        /**
         * @brief Execute rendering
         * Render the input framebuffer content to EGL Window Surface, then swap buffers to display
         */
        void Render() override;

    private:
        SinkSurface();

        // View dimensions
        int view_width_ = 0;      // View width
        int view_height_ = 0;      // View height

        // Display related
        FillMode fill_mode_;       // Fill mode
        bool mirror_ = false;      // Whether to mirror

        // OpenGL related
        GPUPixelGLProgram* display_program_;              // Shader program for display
        uint32_t position_attribute_location_;            // Vertex position attribute location
        uint32_t tex_coord_attribute_location_;           // Texture coordinate attribute location
        uint32_t color_map_uniform_location_;             // Texture uniform location

        // Background color
        struct {
            float r;
            float g;
            float b;
            float a;
        } background_color_;

        float display_vertices_[8];  // Display vertices coordinates (8 floats, 4 vertices with 2 coordinates each)

        // Android Surface related
        ANativeWindow* native_window_ = nullptr;  // Android native window pointer
        EGLSurface egl_surface_ = EGL_NO_SURFACE; // EGL Window Surface
        bool surface_created_ = false;            // Whether Surface has been created

        /**
         * @brief Update display vertices coordinates
         * Calculate vertex coordinates based on input framebuffer dimensions and fill mode
         */
        void UpdateDisplayVertices();

        /**
         * @brief Get texture coordinates
         * @param rotation_mode Rotation mode
         * @return Pointer to texture coordinates array (8 floats, 4 vertices with 2 coordinates each)
         */
        const float* GetTextureCoordinate(RotationMode rotation_mode);

        /**
         * @brief Create EGL Window Surface
         * @return Whether creation was successful
         */
        bool CreateEGLSurface();

        /**
         * @brief Destroy EGL Window Surface
         */
        void DestroyEGLSurface();
#endif
    };

}  // namespace gpupixel
