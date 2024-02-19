/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

public class GPUPixelFilter extends GPUPixelSource implements GPUPixelTarget {
    private String filterClassName;

    static public GPUPixelFilter create(final String filterName) {
        return new GPUPixelFilter(filterName);
    }

    private GPUPixelFilter(final String filterClassName) {
        if (mNativeClassID != 0) return;
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeFilterCreate(filterClassName);
            }
        });
        this.filterClassName = filterClassName;
    }

    public String getFilterClassName() {
        return filterClassName;
    }

    public void setProperty(final String property, final double value){
        setProperty(property, (float)value);
    }

    public void setProperty(final String property, final float value){
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyFloat(mNativeClassID, property, value);
                }
            }
        });
    }

    public void setProperty(final String property, final float[] array){
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyFloatArray(mNativeClassID, property, array);
                }
            }
        });
    }

    public void setProperty(final String property, final int value){
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyInt(mNativeClassID, property, value);
                }
            }
        });
    }

    public void setProperty(final String property, final String value){
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyString(mNativeClassID, property, value);
                }
            }
        });
    }

    public void destroy() {
        destroy(true);
    }

    public void destroy(boolean onGLThread) {
        if (mNativeClassID != 0) {
            if (onGLThread) {
                GPUPixel.getInstance().runOnDraw(new Runnable() {
                    @Override
                    public void run() {
                        if (mNativeClassID != 0) {
                            GPUPixel.nativeFilterDestroy(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    }
                });
            } else {
                GPUPixel.nativeFilterDestroy(mNativeClassID);
                mNativeClassID = 0;
            }
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeClassID != 0) {
                if (GPUPixel.getInstance().getGLSurfaceView() != null) {
                    GPUPixel.getInstance().runOnDraw(new Runnable() {
                        @Override
                        public void run() {
                            GPUPixel.nativeFilterFinalize(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    });
                    GPUPixel.getInstance().requestRender();
                } else {
                    GPUPixel.nativeFilterFinalize(mNativeClassID);
                    mNativeClassID = 0;
                }
            }
        } finally {
            super.finalize();
        }
    }

    //
    public final class BeautyFaceFilter {
        public static final String name = "BeautyFaceFilter";
        public static final String propSmoothLevel = "skin_smoothing";
        public static final String propWhiteLevel = "whiteness";
    }

    public final class FaceReshapeFilter {
        public static final String name = "FaceReshapeFilter";
        public static final String propThinLevel = "thin_face";
        public static final String propBigeyeLevel = "big_eye";
        public static final String propFaceLandmark = "face_landmark";
    }

    public final class LipstickFilter {
        public static final String name = "LipstickFilter";
        public static final String propBlendLevel = "blend_level";
        public static final String propFaceLandmark = "face_landmark";
    }

    public final class BlusherFilter {
        public static final String name = "BlusherFilter";
        public static final String propBlendLevel = "blend_level";
        public static final String propFaceLandmark = "face_landmark";
    }
}
