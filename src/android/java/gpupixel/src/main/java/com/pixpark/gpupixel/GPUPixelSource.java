/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.graphics.Bitmap;

import com.pixpark.gpupixel.filter.GPUPixelFilter;

import java.nio.ByteBuffer;

public abstract class GPUPixelSource {
    protected long mNativeClassID = 0;

    public long getNativeClassID() {
        return mNativeClassID;
    }

    public GPUPixelSource addTarget(GPUPixelTarget target) {
        return addTarget(target, -1);
    }

    public final GPUPixelSource addTarget(final GPUPixelTarget target, final int texID) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeSourceAddTarget(mNativeClassID, target.getNativeClassID(), texID, target instanceof GPUPixelFilter);
            }
        });
        if (target instanceof GPUPixelSource)
            return (GPUPixelSource) target;
        else
            return null;
    }

    public final void removeTarget(final GPUPixelTarget target) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0 && target.getNativeClassID() != 0)
                    GPUPixel.nativeSourceRemoveTarget(mNativeClassID, target.getNativeClassID(), target instanceof GPUPixelFilter);
            }
        });
    }

    public final void removeAllTargets() {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeSourceRemoveAllTargets(mNativeClassID);
            }
        });
    }

    public void proceed() {
        proceed(true, true);
    }

    public void proceed(final boolean bUpdateTargets, final boolean bRequestRender) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeSourceProceed(mNativeClassID, bUpdateTargets);
            }
        });
        if (bRequestRender) {
            GPUPixel.getInstance().requestRender();
        }
    }

    public int getRotatedFramebufferWidth() {
        return GPUPixel.nativeSourceGetRotatedFramebuferWidth(mNativeClassID);
    }

    public int getRotatedFramebufferHeight() {
        return GPUPixel.nativeSourceGetRotatedFramebuferHeight(mNativeClassID);
    }

    public void captureAProcessedFrameData(final GPUPixelFilter upToFilter, final ProcessedFrameDataCallback proceedResult) {
        captureAProcessedFrameData(upToFilter, getRotatedFramebufferWidth(), getRotatedFramebufferHeight(), proceedResult);
    }

    public void captureAProcessedFrameData(final GPUPixelFilter upToFilter, final int width, final int height, final ProcessedFrameDataCallback proceedResult) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    byte[] resultData = GPUPixel.nativeSourceCaptureAProcessedFrameData(mNativeClassID, upToFilter.getNativeClassID(), width, height);
                    if (resultData != null) {
                        Bitmap bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                        bmp.copyPixelsFromBuffer(ByteBuffer.wrap(resultData));
                        proceedResult.onResult(bmp);
                    }
                }
            }
        });
        GPUPixel.getInstance().requestRender();
    }

    public interface ProcessedFrameDataCallback {
        void onResult(Bitmap result);
    }
}
