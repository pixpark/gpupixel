/*
 * GPUPixel
 *
 * Created by PixPark on 2023/10/30.
 * Copyright © 2023 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

/**
 * Face detector class for detecting facial landmarks in images
 */
public class FaceDetector {
    private long mNativeClassID = 0;

    // Image format constants
    public static final int GPUPIXEL_MODE_FMT_VIDEO = 0;
    public static final int GPUPIXEL_MODE_FMT_PICTURE = 1;

    // Frame type constants
    public static final int GPUPIXEL_FRAME_TYPE_YUVI420 = 0;
    public static final int GPUPIXEL_FRAME_TYPE_RGBA = 1;
    public static final int GPUPIXEL_FRAME_TYPE_BGRA = 2;

    /**
     * Create a face detector instance
     */
    protected FaceDetector() {
        mNativeClassID = nativeFaceDetectorCreate();
    }

    /**
     * Create a new FaceDetector instance
     * @return A new FaceDetector instance
     */
    public static FaceDetector Create() {
        return new FaceDetector();
    }

    /**
     * Detect facial landmarks
     * @param data Image data
     * @param width Image width
     * @param height Image height
     * @param format Image format (GPUPIXEL_MODE_FMT_VIDEO or GPUPIXEL_MODE_FMT_PICTURE)
     * @param frameType Frame type (GPUPIXEL_FRAME_TYPE_YUVI420, GPUPIXEL_FRAME_TYPE_RGBA or
     *         GPUPIXEL_FRAME_TYPE_BGRA)
     * @return Array of facial landmark coordinates, each landmark consists of x,y values
     */
    public float[] detect(final byte[] data, final int width, final int height, final int stide,
            final int format, final int frameType) {
        if (mNativeClassID == 0) {
            return new float[0];
        }
        return nativeFaceDetectorDetect(
                mNativeClassID, data, width, height, stide, format, frameType);
    }

    /**
     * Destroy face detector
     */
    public void destroy() {
        if (mNativeClassID != 0) {
            nativeFaceDetectorDestroy(mNativeClassID);
            mNativeClassID = 0;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeClassID != 0) {
                nativeFaceDetectorDestroy(mNativeClassID);
                mNativeClassID = 0;
            }
        } finally {
            super.finalize();
        }
    }

    // Native method declarations
    private static native long nativeFaceDetectorCreate();
    private static native void nativeFaceDetectorDestroy(long classId);
    private static native float[] nativeFaceDetectorDetect(long classId, byte[] data, int width,
            int height, int stride, int format, int frameType);
}
