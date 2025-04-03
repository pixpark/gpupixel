/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel.filter;

import com.pixpark.gpupixel.GPUPixel;
import com.pixpark.gpupixel.GPUPixelSource;
import com.pixpark.gpupixel.GPUPixelSink;

public class GPUPixelFilter extends GPUPixelSource implements GPUPixelSink {
    private String filterClassName;

    GPUPixelFilter(final String filterClassName) {
        if (mNativeClassID != 0) return;
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeFilterCreate(filterClassName);
            }
        });
        this.filterClassName = filterClassName;
    }

    public String GetFilterClassName() {
        return filterClassName;
    }

    public final void SetProperty(final String property, final double value) {
        SetProperty(property, (float) value);
    }

    public final void SetProperty(final String property, final float value) {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyFloat(mNativeClassID, property, value);
                }
            }
        });
    }

    public final void SetProperty(final String property, final float[] array) {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyFloatArray(mNativeClassID, property, array);
                }
            }
        });
    }

    public final void SetProperty(final String property, final int value) {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyInt(mNativeClassID, property, value);
                }
            }
        });
    }

    public final void SetProperty(final String property, final String value) {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyString(mNativeClassID, property, value);
                }
            }
        });
    }

    public final void Destroy() {
        Destroy(true);
    }

    public final void Destroy(boolean onGLThread) {
        if (mNativeClassID != 0) {
            if (onGLThread) {
                GPUPixel.GetInstance().runOnDraw(new Runnable() {
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
                if (GPUPixel.GetInstance().getGLSurfaceView() != null) {
                    GPUPixel.GetInstance().runOnDraw(new Runnable() {
                        @Override
                        public void run() {
                            GPUPixel.nativeFilterFinalize(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    });
                    GPUPixel.GetInstance().requestRender();
                } else {
                    GPUPixel.nativeFilterFinalize(mNativeClassID);
                    mNativeClassID = 0;
                }
            }
        } finally {
            super.finalize();
        }
    }

}
