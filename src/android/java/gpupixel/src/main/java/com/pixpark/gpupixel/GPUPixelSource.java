/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
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

    public GPUPixelSource addSink(GPUPixelSink sink) {
        return addSink(sink, -1);
    }

    public final GPUPixelSource addSink(final GPUPixelSink sink, final int texID) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeSourceAddSink(mNativeClassID, sink.getNativeClassID(), texID, sink instanceof GPUPixelFilter);
            }
        });
        if (sink instanceof GPUPixelSource)
            return (GPUPixelSource) sink;
        else
            return null;
    }

    public final void removeSink(final GPUPixelSink sink) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0 && sink.getNativeClassID() != 0)
                    GPUPixel.nativeSourceRemoveSink(mNativeClassID, sink.getNativeClassID(), sink instanceof GPUPixelFilter);
            }
        });
    }

    public final void removeAllSinks() {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeSourceRemoveAllSinks(mNativeClassID);
            }
        });
    }

    public void doRender() {
        doRender(true, true);
    }

    public void doRender(final boolean updateSinks, final boolean bRequestRender) {
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeSourceProceed(mNativeClassID, updateSinks);
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
