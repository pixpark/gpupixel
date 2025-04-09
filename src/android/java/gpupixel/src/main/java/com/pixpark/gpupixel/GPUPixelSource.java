/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

/**
 * Base source class for GPU image processing pipeline
 */
public class GPUPixelSource {
    protected long mNativeClassID = 0;

    // Base constructor - subclasses must implement their own constructors and set mNativeClassID
    protected GPUPixelSource() {}

    /**
     * Destroys the source and releases resources
     */
    public void Destroy() {
        if (mNativeClassID != 0) {
            // Just reset ID as there's no nativeDestroy implementation
            mNativeClassID = 0;
        }
    }

    /**
     * Adds a sink to this source
     * @param sink The sink object to add
     */
    public final void AddSink(final GPUPixelSink sink) {
        if (mNativeClassID != 0) {
            nativeAddSink(mNativeClassID, sink.getNativeClassID(), sink instanceof GPUPixelFilter);
        }
    }

    /**
     * Removes a sink from this source
     * @param sink The sink to remove
     */
    public final void RemoveSink(final GPUPixelSink sink) {
        if (mNativeClassID != 0 && sink.getNativeClassID() != 0)
            nativeRemoveSink(
                    mNativeClassID, sink.getNativeClassID(), sink instanceof GPUPixelFilter);
    }

    /**
     * Removes all sinks from this source
     */
    public final void RemoveAllSinks() {
        if (mNativeClassID != 0) nativeRemoveAllSinks(mNativeClassID);
    }

    /**
     * Checks if this source contains the specified sink
     * @param sink The sink to check
     * @return true if this source contains the sink
     */
    public final boolean HasSink(final GPUPixelSink sink) {
        if (mNativeClassID != 0 && sink.getNativeClassID() != 0)
            return nativeHasSink(
                    mNativeClassID, sink.getNativeClassID(), sink instanceof GPUPixelFilter);
        return false;
    }

    /**
     * Sets the framebuffer scale
     * @param scale The scale factor
     */
    public final void SetFramebufferScale(float scale) {
        if (mNativeClassID != 0) nativeSetFramebufferScale(mNativeClassID, scale);
    }

    /**
     * Gets the width of the rotated framebuffer
     * @return The width in pixels
     */
    public int GetRotatedFramebufferWidth() {
        if (mNativeClassID != 0) return nativeGetRotatedFramebufferWidth(mNativeClassID);
        return 0;
    }

    /**
     * Gets the height of the rotated framebuffer
     * @return The height in pixels
     */
    public int GetRotatedFramebufferHeight() {
        if (mNativeClassID != 0) return nativeGetRotatedFramebufferHeight(mNativeClassID);
        return 0;
    }

    /**
     * Gets the native class ID for JNI operations
     * @return The native class ID
     */
    public long getSourceNativeClassID() {
        return mNativeClassID;
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeClassID != 0) {
                // Clean up resources by removing all sinks
                RemoveAllSinks();
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            super.finalize();
        }
    }

    // JNI native methods
    private static native long nativeAddSink(long sourceId, long sinkId, boolean isFilter);
    private static native void nativeRemoveSink(long sourceId, long sinkId, boolean isFilter);
    private static native void nativeRemoveAllSinks(long sourceId);
    private static native boolean nativeHasSink(long sourceId, long sinkId, boolean isFilter);
    private static native void nativeSetFramebufferScale(long sourceId, float scale);
    private static native int nativeGetRotatedFramebufferWidth(long sourceId);
    private static native int nativeGetRotatedFramebufferHeight(long sourceId);
}
