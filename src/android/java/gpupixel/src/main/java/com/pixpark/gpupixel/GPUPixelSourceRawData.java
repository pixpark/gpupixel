/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

public class GPUPixelSourceRawData extends GPUPixelSource {
    public GPUPixelSourceRawData() {
        if (mNativeClassID != 0) return;
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeSourceRawDataNew();
            }
        });
    }

    public void SetRotation(int rotation)
    {
        GPUPixel.nativeSourceRawDataSetRotation(mNativeClassID, rotation);
    }

    public void processData(final int[] pixels, int width, int height, int stride) {
        GPUPixel.nativeSourceRawDataUploadBytes(mNativeClassID, pixels, width, height, stride);
        doRender(true, false);
    }

}
