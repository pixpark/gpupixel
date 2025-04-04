package com.pixpark.gpupixel.filter;

/**
 * Lipstick filter
 */
public final class LipstickFilter extends GPUPixelFilter {
    private static final String name = "LipstickFilter";
    private static final String propBlendLevel = "blend_level";
    private static final String propFaceLandmark = "face_landmark";

    private float blendLevel = 0.0f;

    private float[] faceLandmark;

    public LipstickFilter() {
        super(name);
    }

    public float getBlendLevel() {
        return blendLevel;
    }

    public void SetBlendLevel(float blendLevel) {
        this.blendLevel = blendLevel;
        SetProperty(propBlendLevel, blendLevel);
    }

    public float[] getFaceLandmark() {
        return faceLandmark;
    }

    public void setFaceLandmark(float[] faceLandmark) {
        this.faceLandmark = faceLandmark;
        SetProperty(propFaceLandmark, faceLandmark);
    }
}
