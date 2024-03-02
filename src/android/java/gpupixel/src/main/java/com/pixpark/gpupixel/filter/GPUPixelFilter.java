/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel.filter;

import com.pixpark.gpupixel.GPUPixel;
import com.pixpark.gpupixel.GPUPixelSource;
import com.pixpark.gpupixel.GPUPixelTarget;

public class GPUPixelFilter extends GPUPixelSource implements GPUPixelTarget {
    private String filterClassName;

    GPUPixelFilter(final String filterClassName) {
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

    public final void setProperty(final String property, final double value) {
        setProperty(property, (float) value);
    }

    public final void setProperty(final String property, final float value) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyFloat(mNativeClassID, property, value);
                }
            }
        });
    }

    public final void setProperty(final String property, final float[] array) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyFloatArray(mNativeClassID, property, array);
                }
            }
        });
    }

    public final void setProperty(final String property, final int value) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyInt(mNativeClassID, property, value);
                }
            }
        });
    }

    public final void setProperty(final String property, final String value) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    GPUPixel.nativeFilterSetPropertyString(mNativeClassID, property, value);
                }
            }
        });
    }

    public final void destroy() {
        destroy(true);
    }

    public final void destroy(boolean onGLThread) {
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

}
