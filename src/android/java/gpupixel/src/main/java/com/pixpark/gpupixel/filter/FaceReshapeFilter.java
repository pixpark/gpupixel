package com.pixpark.gpupixel.filter;

/**
 * 瘦脸 大眼
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
        setProperty(propThinLevel, thinLevel);
    }

    public float getBigeyeLevel() {
        return bigeyeLevel;
    }

    public void setBigeyeLevel(float bigeyeLevel) {
        this.bigeyeLevel = bigeyeLevel;
        setProperty(propBigeyeLevel, bigeyeLevel);
    }

    public float[] getFaceLandmark() {
        return faceLandmark;
    }

    public void setFaceLandmark(float[] faceLandmark) {
        this.faceLandmark = faceLandmark;
        setProperty(propFaceLandmark, faceLandmark);
    }
}