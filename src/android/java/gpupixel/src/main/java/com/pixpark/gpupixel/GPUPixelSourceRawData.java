/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.content.Context;

import java.nio.ByteBuffer;

public class GPUPixelSourceRawData extends GPUPixelSource {
    // Frame data format types
    public static final int FRAME_TYPE_YUVI420 = 0;
    public static final int FRAME_TYPE_RGBA = 1;
    public static final int FRAME_TYPE_BGRA = 2;

    public GPUPixelSourceRawData() {
        if (mNativeClassID != 0) return;
        mNativeClassID = nativeCreate();
    }

    public static GPUPixelSourceRawData Create() {
        final GPUPixelSourceRawData source = new GPUPixelSourceRawData();
        if (source.mNativeClassID != 0) return source;

        source.mNativeClassID = nativeCreate();
        return source;
    }

    // Set rotation direction
    public void SetRotation(int rotation) {
        nativeSetRotation(mNativeClassID, rotation);
    }

    // Unified data processing interface - accepts only byte[]
    public void ProcessData(byte[] data, int width, int height, int stride, int frameType) {
        nativeProcessData(mNativeClassID, data, width, height, stride, frameType);
    }

    public void Destroy() {
        Destroy(true);
    }

    public void Destroy(boolean onGLThread) {
        if (mNativeClassID != 0) {
            nativeDestroy(mNativeClassID);
            mNativeClassID = 0;
        }
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
    private static native void nativeProcessData(
            long nativeObj, byte[] data, int width, int height, int stride, int frameType);
    private static native void nativeSetRotation(long nativeObj, int rotation);
}
