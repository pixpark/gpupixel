/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.graphics.Bitmap;

public class GPUPixelSourceImage extends GPUPixelSource {
    protected GPUPixelSourceImage() {
        // Empty constructor, instance should be created via static methods
    }

    // Create image source from file
    public static GPUPixelSourceImage CreateFromFile(final String filePath) {
        final GPUPixelSourceImage source = new GPUPixelSourceImage();
        source.mNativeClassID = nativeCreateFromFile(filePath);
        return source;
    }

    // Create image source from byte buffer
    public static GPUPixelSourceImage CreateFromBuffer(
            final byte[] data, final int width, final int height, final int channelCount) {
        final GPUPixelSourceImage source = new GPUPixelSourceImage();
        source.mNativeClassID = nativeCreateFromBuffer(width, height, channelCount, data);
        return source;
    }

    // Create image source from Bitmap
    public static GPUPixelSourceImage CreateFromBitmap(final Bitmap bitmap) {
        final GPUPixelSourceImage source = new GPUPixelSourceImage();
        source.mNativeClassID = nativeCreateFromBitmap(bitmap);
        return source;
    }

    // Get image width
    public int GetWidth() {
        return nativeGetWidth(mNativeClassID);
    }

    // Get image height
    public int GetHeight() {
        return nativeGetHeight(mNativeClassID);
    }

    // Get RGBA image data
    public byte[] GetRgbaImageBuffer() {
        return nativeGetRgbaImageBuffer(mNativeClassID);
    }

    // Execute rendering
    public void Render() {
        nativeRender(mNativeClassID);
    }

    @Override
    public void Destroy() {
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
    private static native long nativeCreateFromFile(String filePath);
    private static native long nativeCreateFromBuffer(
            int width, int height, int channelCount, byte[] data);
    private static native long nativeCreateFromBitmap(Bitmap bitmap);
    private static native void nativeDestroy(long nativeObj);
    private static native void nativeFinalize(long nativeObj);
    private static native int nativeGetWidth(long nativeObj);
    private static native int nativeGetHeight(long nativeObj);
    private static native byte[] nativeGetRgbaImageBuffer(long nativeObj);
    private static native void nativeRender(long nativeObj);
}
