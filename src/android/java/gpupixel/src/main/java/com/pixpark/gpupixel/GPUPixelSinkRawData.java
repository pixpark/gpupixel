/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import java.nio.ByteBuffer;

public class GPUPixelSinkRawData implements GPUPixelSink {
    protected long mNativeClassID = 0;

    protected GPUPixelSinkRawData() {
        if (mNativeClassID != 0) return;
        mNativeClassID = nativeCreate();
    }

    /**
     * Create a new GPUPixelSinkRawData instance
     * @return A new GPUPixelSinkRawData instance
     */
    public static GPUPixelSinkRawData Create() {
        return new GPUPixelSinkRawData();
    }

    // Get width
    public int GetWidth() {
        return nativeGetWidth(mNativeClassID);
    }

    // Get height
    public int GetHeight() {
        return nativeGetHeight(mNativeClassID);
    }

    // Get RGBA buffer
    public byte[] GetRgbaBuffer() {
        return nativeGetRgbaBuffer(mNativeClassID);
    }

    // Get I420 buffer
    public byte[] GetI420Buffer() {
        return nativeGetI420Buffer(mNativeClassID);
    }

    public void Destroy() {
        if (mNativeClassID != 0) {
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

    // JNI Native methods
    private static native long nativeCreate();
    private static native void nativeDestroy(long nativeObj);
    private static native void nativeFinalize(long nativeObj);
    private static native int nativeGetWidth(long nativeObj);
    private static native int nativeGetHeight(long nativeObj);
    private static native byte[] nativeGetRgbaBuffer(long nativeObj);
    private static native byte[] nativeGetI420Buffer(long nativeObj);
}
