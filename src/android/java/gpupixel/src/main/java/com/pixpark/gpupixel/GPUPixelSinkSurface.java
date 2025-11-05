/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.view.Surface;

/**
 * GPUPixelSinkSurface Class - Used to directly render GPU-processed images to Android Surface
 *
 * Function Description:
 * This class is an Android-specific rendering output class that can directly render GPU-processed images to Android Surface,
 * avoiding the overhead of copying RGBA data from C layer to Java layer and rendering again in Java layer.
 *
 * Usage Flow:
 * 1. Call Create() to create an instance
 * 2. Set Surface (via SetSurface method, typically called in TextureView's SurfaceTextureListener)
 * 3. Add SinkSurface to the end of the filter chain
 * 4. Processed images will be automatically rendered to Surface
 *
 * Performance Advantages:
 * - Eliminate data copy: No need to copy RGBA data from C layer to Java layer
 * - Eliminate double rendering: No need to render again in Java layer's GLSurfaceView
 * - Unified EGL context: The entire rendering process is completed in the same EGL context in C layer
 *
 * @see GPUPixelSource
 * @see GPUPixelFilter
 */
public class GPUPixelSinkSurface implements GPUPixelSink {
    protected long mNativeClassID = 0;

    protected GPUPixelSinkSurface() {
        if (mNativeClassID != 0) return;
        mNativeClassID = nativeCreate();
    }

    /**
     * Create a new GPUPixelSinkSurface instance
     * @return GPUPixelSinkSurface instance
     */
    public static GPUPixelSinkSurface Create() {
        return new GPUPixelSinkSurface();
    }

    /**
     * Set Android Surface for rendering
     *
     * Usually called in TextureView's SurfaceTextureListener.onSurfaceTextureAvailable().
     * This method creates EGL Window Surface from Surface and prepares for rendering.
     *
     * @param surface The Android Surface to render to (usually created from TextureView.getSurfaceTexture())
     * @param width Surface width (pixels)
     * @param height Surface height (pixels)
     */
    public void SetSurface(Surface surface, int width, int height) {
        if (mNativeClassID != 0 && surface != null) {
            nativeSetSurface(mNativeClassID, surface, width, height);
        }
    }

    /**
     * Release Surface resources
     *
     * Usually called in TextureView's SurfaceTextureListener.onSurfaceTextureDestroyed().
     * This method destroys EGL Surface and releases ANativeWindow resources.
     */
    public void ReleaseSurface() {
        if (mNativeClassID != 0) {
            nativeReleaseSurface(mNativeClassID);
        }
    }

    // Fill mode constants
    /** Stretch fill mode: Image will be stretched to fill the entire Surface, may be distorted */
    public static final int STRETCH = 0;
    /** Preserve aspect ratio fill mode: Image maintains original aspect ratio, may have black bars */
    public static final int PRESERVE_ASPECT_RATIO = 1;
    /** Preserve aspect ratio and fill mode: Image maintains original aspect ratio and scales to fill, may crop some content */
    public static final int PRESERVE_ASPECT_RATIO_AND_FILL = 2;

    /**
     * Set fill mode
     * @param fillMode Fill mode (STRETCH, PRESERVE_ASPECT_RATIO or PRESERVE_ASPECT_RATIO_AND_FILL)
     */
    public void SetFillMode(int fillMode) {
        if (mNativeClassID != 0) {
            nativeSetFillMode(mNativeClassID, fillMode);
        }
    }

    /**
     * Set mirror mode
     * @param mirror true for horizontal mirror (left-right flip), false for normal display
     *
     * Commonly used for front camera preview, as front camera usually needs mirror display
     */
    public void SetMirror(boolean mirror) {
        if (mNativeClassID != 0) {
            nativeSetMirror(mNativeClassID, mirror);
        }
    }

    /**
     * Destroy SinkSurface and release all resources
     *
     * Called in Activity or Fragment's onDestroy() to ensure resources are properly released.
     */
    public void Destroy() {
        if (mNativeClassID != 0) {
            ReleaseSurface();
            nativeDestroy(mNativeClassID);
            mNativeClassID = 0;
        }
    }

    @Override
    public long getNativeClassID() {
        return mNativeClassID;
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeClassID != 0) {
                nativeFinalize(mNativeClassID);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            super.finalize();
        }
    }

    // ========== JNI Native Method Declarations ==========
    // These methods are implemented by C++ layer through JNI, corresponding to functions in jni_sink_surface.cc

    /** Create C++ layer SinkSurface object, return object pointer */
    private static native long nativeCreate();

    /** Destroy C++ layer SinkSurface object */
    private static native void nativeDestroy(long nativeObj);

    /** Finalize resources, called in Java object's finalize() */
    private static native void nativeFinalize(long nativeObj);

    /** Set Android Surface */
    private static native void nativeSetSurface(long nativeObj, Surface surface, int width, int height);

    /** Release Surface resources */
    private static native void nativeReleaseSurface(long nativeObj);

    /** Set fill mode */
    private static native void nativeSetFillMode(long nativeObj, int fillMode);

    /** Set mirror mode */
    private static native void nativeSetMirror(long nativeObj, boolean mirror);
}
