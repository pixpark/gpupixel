/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.graphics.Bitmap;
import java.nio.ByteBuffer;

public class GPUPixelSourceRawInput extends GPUPixelSource {
    public GPUPixelSourceRawInput() {
        if (mNativeClassID != 0) return;
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeSourceRawInputNew();
            }
        });
    }

    public void SetRotation(int rotation)
    {
        GPUPixel.nativeSourceRawInputSetRotation(mNativeClassID, rotation);
    }

    public void uploadBytes(final int[] pixels, int width, int height, int stride) {
        GPUPixel.nativeSourceRawInputUploadBytes(mNativeClassID, pixels, width, height, stride);
        proceed(true, false);
    }

}
