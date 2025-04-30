/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

public class GPUPixelFilter extends GPUPixelSource implements GPUPixelSink {
    private String filterClassName;

    // Beauty-related filters
    public static final String BEAUTY_FACE_FILTER = "BeautyFaceFilter";
    public static final String FACE_RESHAPE_FILTER = "FaceReshapeFilter";
    public static final String LIPSTICK_FILTER = "LipstickFilter";
    public static final String BLUSHER_FILTER = "BlusherFilter";
    public static final String FACE_MAKEUP_FILTER = "FaceMakeupFilter";

    // Basic adjustment filters
    public static final String CONTRAST_FILTER = "ContrastFilter";
    public static final String EXPOSURE_FILTER = "ExposureFilter";
    public static final String SATURATION_FILTER = "SaturationFilter";
    public static final String RGB_FILTER = "RGBFilter";
    public static final String HUE_FILTER = "HueFilter";
    public static final String HSB_FILTER = "HSBFilter";
    public static final String COLOR_INVERT_FILTER = "ColorInvertFilter";
    public static final String WHITE_BALANCE_FILTER = "WhiteBalanceFilter";
    public static final String COLOR_MATRIX_FILTER = "ColorMatrixFilter";

    // Blur filters
    public static final String IOS_BLUR_FILTER = "IOSBlurFilter";
    public static final String BILATERAL_FILTER = "BilateralFilter";

    // Edge detection and effect filters
    public static final String SOBEL_EDGE_DETECTION_FILTER = "SobelEdgeDetectionFilter";
    public static final String CANNY_EDGE_DETECTION_FILTER = "CannyEdgeDetectionFilter";
    public static final String DIRECTIONAL_NON_MAXIMUM_SUPPRESSION_FILTER =
            "DirectionalNonMaximumSuppressionFilter";
    public static final String WEAK_PIXEL_INCLUSION_FILTER = "WeakPixelInclusionFilter";

    // Special effect filters
    public static final String TOON_FILTER = "ToonFilter";
    public static final String SMOOTH_TOON_FILTER = "SmoothToonFilter";
    public static final String POSTERIZE_FILTER = "PosterizeFilter";
    public static final String PIXELLATION_FILTER = "PixellationFilter";
    public static final String SKETCH_FILTER = "SketchFilter";
    public static final String GLASS_SPHERE_FILTER = "GlassSphereFilter";
    public static final String SPHERE_REFRACTION_FILTER = "SphereRefractionFilter";
    public static final String EMBOSS_FILTER = "EmbossFilter";

    // Other processing filters
    public static final String LUMINANCE_RANGE_FILTER = "LuminanceRangeFilter";

    protected GPUPixelFilter(final String filterClassName) {
        if (mNativeClassID != 0) return;
        mNativeClassID = nativeFilterCreate(filterClassName);
        this.filterClassName = filterClassName;
    }

    /**
     * Create a new GPUPixelFilter instance
     * @param filterClassName Filter class name
     * @return Newly created GPUPixelFilter instance
     */
    public static GPUPixelFilter Create(final String filterClassName) {
        return new GPUPixelFilter(filterClassName);
    }

    public String GetFilterClassName() {
        return filterClassName;
    }

    public final boolean SetProperty(final String property, final double value) {
        return SetProperty(property, (float) value);
    }

    public final boolean SetProperty(final String property, final float value) {
        if (mNativeClassID != 0) {
            return nativeFilterSetPropertyFloat(mNativeClassID, property, value);
        }
        return false;
    }

    public final boolean SetProperty(final String property, final float[] array) {
        if (mNativeClassID != 0) {
            return nativeFilterSetPropertyFloatArray(mNativeClassID, property, array);
        }
        return false;
    }

    public final boolean SetProperty(final String property, final int value) {
        if (mNativeClassID != 0) {
            return nativeFilterSetPropertyInt(mNativeClassID, property, value);
        }
        return false;
    }

    public final boolean SetProperty(final String property, final String value) {
        if (mNativeClassID != 0) {
            return nativeFilterSetPropertyString(mNativeClassID, property, value);
        }
        return false;
    }

    @Override
    public long getNativeClassID() {
        return mNativeClassID;
    }

    @Override
    public final void Destroy() {
        if (mNativeClassID != 0) {
            nativeFilterDestroy(mNativeClassID);
            mNativeClassID = 0;
        }
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeClassID != 0) {
                nativeFilterFinalize(mNativeClassID);
                mNativeClassID = 0;
            }
        } finally {
            super.finalize();
        }
    }

    // Native method declarations
    private static native long nativeFilterCreate(String filterClassName);
    private static native void nativeFilterDestroy(long classId);
    private static native void nativeFilterFinalize(long classId);
    private static native boolean nativeFilterSetPropertyFloat(
            long classId, String property, float value);
    private static native boolean nativeFilterSetPropertyInt(long classId, String property, int value);
    private static native boolean nativeFilterSetPropertyString(
            long classId, String property, String value);
    private static native boolean nativeFilterSetPropertyFloatArray(
            long classId, String property, float[] array);

    private static native float nativeFilterGetPropertyFloat(long classId, String property);
    private static native int nativeFilterGetPropertyInt(long classId, String property);
    private static native String nativeFilterGetPropertyString(long classId, String property);

    private static native boolean nativeFilterHasProperty(long classId, String property, String type);
    public final boolean HasProperty(final String property, final String type) {
        if (mNativeClassID != 0) {
            return nativeFilterHasProperty(mNativeClassID, property, type);
        }
        return false;
    }

    public final int GetPropertyInt(final String property) {
        if (mNativeClassID == 0) {
            return 0;
        }
        return nativeFilterGetPropertyInt(mNativeClassID, property);
    }
    public final float GetPropertyFloat(final String property) {
        if (mNativeClassID == 0) {
            return 0;
        }
        return nativeFilterGetPropertyFloat(mNativeClassID, property);
    }
    public final String GetPropertyString(final String property) {
        if (mNativeClassID == 0) {
            return null;
        }
        return nativeFilterGetPropertyString(mNativeClassID, property);
    }
}
