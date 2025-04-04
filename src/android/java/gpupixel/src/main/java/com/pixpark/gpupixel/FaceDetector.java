/*
 * GPUPixel
 *
 * Created by PixPark on 2023/10/30.
 * Copyright © 2023 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

public class FaceDetector {
    private long mNativeClassID = 0;

    // Image format constants
    public static final int GPUPIXEL_MODE_FMT_VIDEO = 0;
    public static final int GPUPIXEL_MODE_FMT_PICTURE = 1;

    // Frame type constants
    public static final int GPUPIXEL_FRAME_TYPE_YUVI420 = 0;
    public static final int GPUPIXEL_FRAME_TYPE_RGBA = 1;
    public static final int GPUPIXEL_FRAME_TYPE_BGRA = 2;

    public FaceDetector() {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeFaceDetectorCreate();
            }
        });
    }

    public long getNativeClassID() {
        return mNativeClassID;
    }

    /**
     * Detect face landmarks
     * @param data Image data
     * @param width Image width
     * @param height Image height
     * @param format Image format (GPUPIXEL_MODE_FMT_VIDEO or GPUPIXEL_MODE_FMT_PICTURE)
     * @param frameType Frame type (GPUPIXEL_FRAME_TYPE_YUVI420, GPUPIXEL_FRAME_TYPE_RGBA or
     *         GPUPIXEL_FRAME_TYPE_BGRA)
     * @return Face landmark coordinate array
     */
    public float[] detect(final byte[] data, final int width, final int height, final int format,
            final int frameType) {
        return GPUPixel.nativeFaceDetectorDetect(
                mNativeClassID, data, width, height, format, frameType);
    }

    /**
     * Destroy face detector
     */
    public void destroy() {
        if (mNativeClassID != 0) {
            GPUPixel.GetInstance().runOnDraw(new Runnable() {
                @Override
                public void run() {
                    if (mNativeClassID != 0) {
                        GPUPixel.nativeFaceDetectorDestroy(mNativeClassID);
                        mNativeClassID = 0;
                    }
                }
            });
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeClassID != 0) {
                if (GPUPixel.GetInstance().getGLSurfaceView() != null) {
                    GPUPixel.GetInstance().runOnDraw(new Runnable() {
                        @Override
                        public void run() {
                            GPUPixel.nativeFaceDetectorDestroy(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    });
                    GPUPixel.GetInstance().requestRender();
                } else {
                    GPUPixel.nativeFaceDetectorDestroy(mNativeClassID);
                    mNativeClassID = 0;
                }
            }
        } finally {
            super.finalize();
        }
    }
}
