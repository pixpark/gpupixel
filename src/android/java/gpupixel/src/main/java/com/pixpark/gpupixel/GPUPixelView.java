/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.widget.FrameLayout;


public class GPUPixelView extends FrameLayout implements GPUPixelTarget {
    static final int FillModeStretch = 0;                   // Stretch to fill the view, and may distort the image
    static final int FillModePreserveAspectRatio = 1;       // preserve the aspect ratio of the image
    static final int FillModePreserveAspectRatioAndFill =2; // preserve the aspect ratio, and zoom in to fill the view

    protected long mNativeClassID = 0;

    private GLSurfaceView mGLSurfaceView;

    public GPUPixelView(Context context) {
        super(context);
        init(context, null);
    }

    public GPUPixelView(Context context, AttributeSet attrs) {
        super(context, attrs);
        if (isInEditMode()) { //防止布局界面显示为空白
            return;
        }
        init(context, attrs);
    }

    private void init(Context context, AttributeSet attrs) {
        if (mNativeClassID != 0) return;
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeTargetViewNew();
            }
        });

        mGLSurfaceView = new GPUImageViewGLSurfaceView(context, attrs, this);
        GPUPixel.getInstance().setGLSurfaceView(mGLSurfaceView);
        addView(mGLSurfaceView);
        if (mGLSurfaceView.getWidth() != 0 && mGLSurfaceView.getHeight() != 0) {
            onSurfaceSizeChanged(mGLSurfaceView.getWidth(), mGLSurfaceView.getHeight());
        }
    }

    public long getNativeClassID() { return mNativeClassID; }

    protected void onSurfaceSizeChanged(final int w, final int h) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeTargetViewOnSizeChanged(mNativeClassID, w, h);
            }
        });
    }

    public void setFillMode(final int fillMode) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeTargetViewSetFillMode(mNativeClassID, fillMode);
            }
        });
    }

    public void setMirror(final boolean mirror) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeTargetViewSetMirror(mNativeClassID, mirror);
            }
        });
    }

    public int getSurfaceWidth() {
        return mGLSurfaceView.getWidth();
    }

    public int getSurfaceHeight() {
        return mGLSurfaceView.getHeight();
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeClassID != 0) {
                if (GPUPixel.getInstance().getGLSurfaceView() != null) {
                    GPUPixel.getInstance().runOnDraw(new Runnable() {
                        @Override
                        public void run() {
                            GPUPixel.nativeTargetViewFinalize(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    });
                    GPUPixel.getInstance().requestRender();
                } else {
                    GPUPixel.nativeTargetViewFinalize(mNativeClassID);
                    mNativeClassID = 0;
                }
            }
        } finally {
            super.finalize();
        }
    }

    private class GPUImageViewGLSurfaceView extends GLSurfaceView {
        private GPUPixelView host;

        public GPUImageViewGLSurfaceView(Context context, AttributeSet attrs, GPUPixelView host) {
            super(context, attrs);
            this.host = host;
        }

        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            super.surfaceCreated(holder);
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int w, int h) {
            super.surfaceChanged(holder, format, w, h);
            host.onSurfaceSizeChanged(w, h);
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            super.surfaceDestroyed(holder);
        }
    }

}
