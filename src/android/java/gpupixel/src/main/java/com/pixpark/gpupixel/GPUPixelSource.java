/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.graphics.Bitmap;

import java.nio.ByteBuffer;

public class GPUPixelSource {
    protected long mNativeClassID = 0;

    public GPUPixelSource() {
        if (mNativeClassID != 0) return;
        mNativeClassID = nativeCreate();
    }

    // Execute rendering
    public void Render() {
        nativeRender(mNativeClassID);
    }

    // Get width
    public int GetWidth() {
        return nativeGetWidth(mNativeClassID);
    }

    // Get height
    public int GetHeight() {
        return nativeGetHeight(mNativeClassID);
    }

    // Set output size
    public void SetOutputSize(int width, int height) {
        nativeSetOutputSize(mNativeClassID, width, height);
    }

    // Set rotation
    public void SetRotation(int rotation) {
        nativeSetRotation(mNativeClassID, rotation);
    }

    // Set flip
    public void SetFlip(boolean flipHorizontal, boolean flipVertical) {
        nativeSetFlip(mNativeClassID, flipHorizontal, flipVertical);
    }

    // Set scale type
    public void SetScaleType(int scaleType) {
        nativeSetScaleType(mNativeClassID, scaleType);
    }

    // Set background color
    public void SetBackgroundColor(float r, float g, float b, float a) {
        nativeSetBackgroundColor(mNativeClassID, r, g, b, a);
    }

    // Set output
    public void SetOutput(GPUPixelSink sink) {
        nativeSetOutput(mNativeClassID, sink.getNativeClassID());
    }

    // Remove output
    public void RemoveOutput(GPUPixelSink sink) {
        nativeRemoveOutput(mNativeClassID, sink.getNativeClassID());
    }

    // Remove all outputs
    public void RemoveAllOutput() {
        nativeRemoveAllOutput(mNativeClassID);
    }

    // Destroy
    public void Destroy() {
        Destroy(true);
    }

    // Destroy with option to specify if on GL thread
    public void Destroy(boolean onGLThread) {
        if (mNativeClassID != 0) {
            nativeDestroy(mNativeClassID);
            mNativeClassID = 0;
        }
    }

    // Get native class ID
    public long getNativeClassID() {
        return mNativeClassID;
    }

    @Override
    protected void finalize() throws Throwable {
        try {
            if (mNativeClassID != 0) {
                nativeFinalize(mNativeClassID);
            }
        } catch (Exception e) {
            e.printStackTrace();
        } finally {
            super.finalize();
        }
    }

    public GPUPixelSource AddSink(GPUPixelSink sink) {
        return AddSink(sink, -1);
    }

    public final GPUPixelSource AddSink(final GPUPixelSink sink, final int texID) {
        if (mNativeClassID != 0)
            nativeAddSink(
                    mNativeClassID, sink.getNativeClassID(), texID, sink instanceof GPUPixelFilter);
        if (sink instanceof GPUPixelSource)
            return (GPUPixelSource) sink;
        else
            return null;
    }

    public final void RemoveSink(final GPUPixelSink sink) {
        if (mNativeClassID != 0 && sink.getNativeClassID() != 0)
            nativeRemoveSink(
                    mNativeClassID, sink.getNativeClassID(), sink instanceof GPUPixelFilter);
    }

    public final void RemoveAllSinks() {
        if (mNativeClassID != 0) nativeRemoveAllSinks(mNativeClassID);
    }

    public void DoRender() {
        DoRender(true, true);
    }

    public void DoRender(final boolean updateSinks, final boolean bRequestRender) {
        if (mNativeClassID != 0) nativeProceed(mNativeClassID, updateSinks);
        if (bRequestRender) {
        }
    }

    public int GetRotatedFramebufferWidth() {
        return nativeGetRotatedFramebufferWidth(mNativeClassID);
    }

    public int GetRotatedFramebufferHeight() {
        return nativeGetRotatedFramebufferHeight(mNativeClassID);
    }

    public void GetProcessedFrameData(
            final GPUPixelFilter upToFilter, final ProcessedFrameDataCallback proceedResult) {
        GetProcessedFrameData(upToFilter, GetRotatedFramebufferWidth(),
                GetRotatedFramebufferHeight(), proceedResult);
    }

    public void GetProcessedFrameData(final GPUPixelFilter upToFilter, final int width,
            final int height, final ProcessedFrameDataCallback proceedResult) {
        if (mNativeClassID != 0) {
            byte[] resultData = nativeGetProcessedFrameData(
                    mNativeClassID, upToFilter.getNativeClassID(), width, height);
            if (resultData != null) {
                Bitmap bmp = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                bmp.copyPixelsFromBuffer(ByteBuffer.wrap(resultData));
                proceedResult.onResult(bmp);
            }
        }
    }

    public interface ProcessedFrameDataCallback {
        void onResult(Bitmap result);
    }

    // JNI Native methods
    private static native long nativeCreate();
    private static native void nativeDestroy(long nativeObj);
    private static native void nativeFinalize(long nativeObj);
    private static native void nativeRender(long nativeObj);
    private static native int nativeGetWidth(long nativeObj);
    private static native int nativeGetHeight(long nativeObj);
    private static native void nativeSetOutputSize(long nativeObj, int width, int height);
    private static native void nativeSetRotation(long nativeObj, int rotation);
    private static native void nativeSetFlip(
            long nativeObj, boolean flipHorizontal, boolean flipVertical);
    private static native void nativeSetScaleType(long nativeObj, int scaleType);
    private static native void nativeSetBackgroundColor(
            long nativeObj, float r, float g, float b, float a);
    private static native void nativeSetOutput(long nativeObj, long sinkNativeObj);
    private static native void nativeRemoveOutput(long nativeObj, long sinkNativeObj);
    private static native void nativeRemoveAllOutput(long nativeObj);
    private static native long nativeAddSink(
            long sourceId, long sinkId, int texId, boolean isFilter);
    private static native void nativeRemoveSink(long sourceId, long sinkId, boolean isFilter);
    private static native void nativeRemoveAllSinks(long sourceId);
    private static native boolean nativeProceed(long sourceId, boolean updateSinks);
    private static native int nativeGetRotatedFramebufferWidth(long sourceId);
    private static native int nativeGetRotatedFramebufferHeight(long sourceId);
    private static native byte[] nativeGetProcessedFrameData(
            long sourceId, long upToFilterId, int width, int height);
}
