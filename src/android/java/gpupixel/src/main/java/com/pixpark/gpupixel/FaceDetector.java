/*
 * GPUPixel
 *
 * Created by PixPark on 2023/10/30.
 * Copyright © 2023 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

public class FaceDetector {
    private long mNativeClassID = 0;
    
    // 图像格式常量
    public static final int GPUPIXEL_MODE_FMT_VIDEO = 0;
    public static final int GPUPIXEL_MODE_FMT_PICTURE = 1;
    
    // 帧类型常量
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
     * 检测人脸特征点
     * @param data 图像数据
     * @param width 图像宽度
     * @param height 图像高度
     * @param format 图像格式 (GPUPIXEL_MODE_FMT_VIDEO 或 GPUPIXEL_MODE_FMT_PICTURE)
     * @param frameType 帧类型 (GPUPIXEL_FRAME_TYPE_YUVI420, GPUPIXEL_FRAME_TYPE_RGBA 或 GPUPIXEL_FRAME_TYPE_BGRA)
     * @return 人脸特征点坐标数组
     */
    public float[] detect(final byte[] data, final int width, final int height, final int format, final int frameType) {
        return GPUPixel.nativeFaceDetectorDetect(mNativeClassID, data, width, height, format, frameType);
    }
    
    /**
     * 销毁人脸检测器
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