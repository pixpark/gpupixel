package com.pixpark.gpupixel;

import java.nio.ByteBuffer;

public class GPUPixelSinkRawYuv implements GPUPixelSink {
    protected long mNativeClassID = 0;

    protected GPUPixelSinkRawYuv() {
        if (mNativeClassID != 0) return;
        mNativeClassID = nativeCreate();
    }

    /**
     * Create a new GPUPixelSinkRawData instance
     * @return A new GPUPixelSinkRawData instance
     */
    public static GPUPixelSinkRawYuv create() {
        return new GPUPixelSinkRawYuv();
    }

    // Get width
    public int getWidth() {
        return nativeGetWidth(mNativeClassID);
    }

    // Get height
    public int getHeight() {
        return nativeGetHeight(mNativeClassID);
    }

    // Get I420 buffer
    public ByteBuffer getI420Buffer() {
        return nativeGetI420Buffer(mNativeClassID);
    }

    public void destroy() {
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
    private static native ByteBuffer nativeGetI420Buffer(long nativeObj);

}
