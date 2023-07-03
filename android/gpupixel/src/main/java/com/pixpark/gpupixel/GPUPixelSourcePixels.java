/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.graphics.Bitmap;

public class GPUPixelSourcePixels extends GPUPixelSource {

    protected  Bitmap bitmap;
    public GPUPixelSourcePixels(Bitmap bitmap) {
        if (mNativeClassID != 0) return;
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeSourceImageNew();
            }
        });
        setImage(bitmap);
    }

    public void setImage(final Bitmap bitmap) {
        this.bitmap = bitmap;
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeSourceImageSetImage(mNativeClassID, bitmap);
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
                            GPUPixel.nativeSourceImageDestroy(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    }
                });
            } else {
                GPUPixel.nativeSourceImageDestroy(mNativeClassID);
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
                            GPUPixel.nativeSourceImageFinalize(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    });
                    GPUPixel.getInstance().requestRender();
                } else {
                    GPUPixel.nativeSourceImageFinalize(mNativeClassID);
                    mNativeClassID = 0;
                }
            }
        } finally {
            super.finalize();
        }
    }
}
