/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.graphics.PixelFormat;
import android.os.Build;

public class GPUPixel {
    public static final int NoRotation = 0;
    public static final int RotateLeft = 1;
    public static final int RotateRight = 2;
    public static final int FlipVertical = 3;
    public static final int FlipHorizontal = 4;
    public static final int RotateRightFlipVertical = 5;
    public static final int RotateRightFlipHorizontal = 6;
    public static final int Rotate180 = 7;

    private GPUPixelRenderer mRenderer = null;
    private GLSurfaceView mGLSurfaceView = null;
    private int mGLSurfaceViewRenderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY;

    private static class SingletonHolder {
        private static final GPUPixel INSTANCE = new GPUPixel();
    }


    private GPUPixel(){ init(); }
    public static final GPUPixel getInstance() {
        GPUPixel instance = SingletonHolder.INSTANCE;
        if (!instance.isInited()) {
            instance.init();
        }
        return instance;
    }

    public boolean isInited() {
        return mRenderer != null;
    }

    public void init() {
        mRenderer = new GPUPixelRenderer();
        runOnDraw(new Runnable() {
            @Override
            public void run() {
                nativeContextInit();
            }
        });
    }

    public void destroy() {
        if (!isInited()) return;
        purge();
        setGLSurfaceView(null);
        setSource(null);
        mRenderer.clear();
        mRenderer = null;
    }

    public void setSource(GPUPixelSource source) {
        mRenderer.setSource(source);
    }

    public void setGLSurfaceView(final GLSurfaceView view) {
        mGLSurfaceView = view;
        if (mGLSurfaceView != null) {
            mGLSurfaceView.setEGLContextClientVersion(2);
            mGLSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
            mGLSurfaceView.getHolder().setFormat(PixelFormat.RGBA_8888);
            mGLSurfaceView.setRenderer(mRenderer);
            mGLSurfaceView.setRenderMode(mGLSurfaceViewRenderMode);
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.GINGERBREAD_MR1) {
                mGLSurfaceView.setPreserveEGLContextOnPause(true);
            }
            mGLSurfaceView.requestRender();
        } else {
            mGLSurfaceViewRenderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY;
        }
    }

    public void setGLSurfaceViewRenderMode(int glSurfaceViewRenderMode) {
        mGLSurfaceViewRenderMode = glSurfaceViewRenderMode;
        if (mGLSurfaceView != null) {
            mGLSurfaceView.setRenderMode(mGLSurfaceViewRenderMode);
        }
    }

    public GLSurfaceView getGLSurfaceView() {
        return mGLSurfaceView;
    }

    public void requestRender() {
        if (mGLSurfaceView != null) {
            mGLSurfaceView.requestRender();
        }
    }

    public void purge() {
        if (mGLSurfaceView != null) {
            GPUPixel.getInstance().runOnDraw(new Runnable() {
                @Override
                public void run() {
                    GPUPixel.nativeContextPurge();
                }
            });
            mGLSurfaceView.requestRender();
        } else {
            GPUPixel.nativeContextPurge();
        }
    }

    public GPUPixelRenderer getRenderer() {
        return mRenderer;
    }

    public boolean isPreDrawQueueEmpty() { return mRenderer.isPreDrawQueueEmpty(); }

    public void runOnPreDraw(Runnable runnable) {
        mRenderer.runOnPreDraw(runnable);
    }

    public boolean isDrawQueueEmpty() { return mRenderer.isDrawQueueEmpty(); }

    public void runOnDraw(Runnable runnable) {
        mRenderer.runOnDraw(runnable);
    }

    public boolean isPostDrawQueueEmpty() { return mRenderer.isPostDrawQueueEmpty(); }

    public void runOnPostDraw(Runnable runnable) {
        mRenderer.runOnPostDraw(runnable);
    }



    static {
        System.loadLibrary("gpupixel");
    }

    // Filter
    public static native long nativeFilterCreate(final String filterClassName);
    public static native void nativeFilterDestroy(long classID);
    public static native void nativeFilterFinalize(long classID);
    public static native void nativeFilterSetPropertyFloat(long classID, String property, float value);
    public static native void nativeFilterSetPropertyInt(long classID, String property, int value);
    public static native void nativeFilterSetPropertyString(long classID, String prooerty, String value);

    // SourceImage
    public static native long nativeSourceImageNew();
    public static native void nativeSourceImageDestroy(final long classID);
    public static native void nativeSourceImageFinalize(final long classID);
    public static  native void nativeSourceImageSetImage(final long classID, final Bitmap bitmap);

    // SourceCamera
    public static native long nativeSourceCameraNew();
    public static native void nativeSourceCameraDestroy(final long classID);
    public static native void nativeSourceCameraFinalize(final long classID);
    public static native void nativeSourceCameraSetFrame(final long classID, final int width, final int height, final int[] data, final int rotation);

    // SourceRawDataInput
    public static native long nativeSourceRawInputNew();
    public static native void nativeSourceRawInputUploadBytes(final long classID, final int[] pixel, final int width, final int height, final int stride);
    public static native void nativeSourceRawInputSetRotation(final long classID, final int rotation);

    // Source
    public static native long nativeSourceAddTarget(final long classID, final long targetClassID, final int texID, final boolean isFilter);
    public static native boolean nativeSourceRemoveTarget(final long classID, final long targetClassID, final boolean isFilter);
    public static native boolean nativeSourceRemoveAllTargets(final long classID);
    public static native boolean nativeSourceProceed(final long classID, final boolean bUpdateTargets);
    public static native int nativeSourceGetRotatedFramebuferWidth(final long classID);
    public static native int nativeSourceGetRotatedFramebuferHeight(final long classID);
    public static native byte[] nativeSourceCaptureAProcessedFrameData(final long classId, final long upToFilterClassId, final int width, final int height);

    // view
    public static native long nativeTargetViewNew();
    public static native void nativeTargetViewFinalize(final long classID);
    public static native void nativeTargetViewOnSizeChanged(final long classID, final int width, final int height);
    public static native void nativeTargetViewSetFillMode(final long classID, final int fillMode);
    // context
    public static native void nativeContextInit();
    public static native void nativeContextDestroy();
    public static native void nativeContextPurge();

    // utils
    public static native void nativeYUVtoRBGA(byte[] yuv, int width, int height, int[] out);

}
