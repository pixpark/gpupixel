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

    public GPUPixelSource AddSink(GPUPixelSink sink) {
        return AddSink(sink, -1);
    }

    public final GPUPixelSource AddSink(final GPUPixelSink sink, final int texID) {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0)
                    GPUPixel.nativeSourceAddSink(mNativeClassID, sink.getNativeClassID(), texID,
                            sink instanceof GPUPixelFilter);
            }
        });
        if (sink instanceof GPUPixelSource)
            return (GPUPixelSource) sink;
        else
            return null;
    }

    public final void RemoveSink(final GPUPixelSink sink) {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0 && sink.getNativeClassID() != 0)
                    GPUPixel.nativeSourceRemoveSink(mNativeClassID, sink.getNativeClassID(),
                            sink instanceof GPUPixelFilter);
            }
        });
    }

    public final void RemoveAllSinks() {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) GPUPixel.nativeSourceRemoveAllSinks(mNativeClassID);
            }
        });
    }

    public void DoRender() {
        DoRender(true, true);
    }

    public void DoRender(final boolean updateSinks, final boolean bRequestRender) {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) GPUPixel.nativeSourceProceed(mNativeClassID, updateSinks);
            }
        });
        if (bRequestRender) {
            GPUPixel.GetInstance().requestRender();
        }
    }

    public int GetRotatedFramebufferWidth() {
        return GPUPixel.nativeSourceGetRotatedFramebuferWidth(mNativeClassID);
    }

    public int GetRotatedFramebufferHeight() {
        return GPUPixel.nativeSourceGetRotatedFramebuferHeight(mNativeClassID);
    }

    public void GetProcessedFrameData(
            final GPUPixelFilter upToFilter, final ProcessedFrameDataCallback proceedResult) {
        GetProcessedFrameData(upToFilter, GetRotatedFramebufferWidth(),
                GetRotatedFramebufferHeight(), proceedResult);
    }

    public void GetProcessedFrameData(final GPUPixelFilter upToFilter, final int width,
            final int height, final ProcessedFrameDataCallback proceedResult) {
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    byte[] resultData = GPUPixel.nativeSourceGetProcessedFrameData(
                            mNativeClassID, upToFilter.getNativeClassID(), width, height);
                    if (resultData != null) {
                        Bitmap bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                        bmp.copyPixelsFromBuffer(ByteBuffer.wrap(resultData));
                        proceedResult.onResult(bmp);
                    }
                }
            }
        });
        GPUPixel.GetInstance().requestRender();
    }

    public interface ProcessedFrameDataCallback {
        void onResult(Bitmap result);
    }
}
