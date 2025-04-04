package com.pixpark.gpupixel.filter;

/**
 * Face reshape and eye enlargement filter
 */
public final class FaceReshapeFilter extends GPUPixelFilter {
    private static final String name = "FaceReshapeFilter";
    private static final String propThinLevel = "thin_face";
    private static final String propBigeyeLevel = "big_eye";
    private static final String propFaceLandmark = "face_landmark";

    private float thinLevel = 0.0f;
    private float bigeyeLevel = 0.0f;

    private float[] faceLandmark;

    public FaceReshapeFilter() {
        super(name);
    }

    public float getThinLevel() {
        return thinLevel;
    }

    public void setThinLevel(float thinLevel) {
        this.thinLevel = thinLevel;
        SetProperty(propThinLevel, thinLevel);
    }

    public float getBigeyeLevel() {
        return bigeyeLevel;
    }

    public void setBigeyeLevel(float bigeyeLevel) {
        this.bigeyeLevel = bigeyeLevel;
        SetProperty(propBigeyeLevel, bigeyeLevel);
    }

    public float[] getFaceLandmark() {
        return faceLandmark;
    }

    public void setFaceLandmark(float[] faceLandmark) {
        this.faceLandmark = faceLandmark;
        SetProperty(propFaceLandmark, faceLandmark);
    }
}
