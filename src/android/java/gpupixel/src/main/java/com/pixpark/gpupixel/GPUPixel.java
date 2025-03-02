/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.content.Context;
import android.graphics.Bitmap;
import android.opengl.GLSurfaceView;
import android.graphics.PixelFormat;
import android.os.Build;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class GPUPixel {

    public interface GPUPixelLandmarkCallback {
        public void onFaceLandmark(float[] landmarks);
    }
    public static final int NoRotation = 0;
    public static final int RotateLeft = 1;
    public static final int RotateRight = 2;
    public static final int FlipVertical = 3;
    public static final int FlipHorizontal = 4;
    public static final int RotateRightFlipVertical = 5;
    public static final int RotateRightFlipHorizontal = 6;
    public static final int Rotate180 = 7;

    public static String resource_path;
    private GPUPixelRenderer mRenderer = null;
    private GLSurfaceView mGLSurfaceView = null;
    private int mGLSurfaceViewRenderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY;

    private static class SingletonHolder {
        private static final GPUPixel INSTANCE = new GPUPixel();
    }


    private GPUPixel(){ init(); }
    public static final GPUPixel getInstance() {
        GPUPixel instance = SingletonHolder.INSTANCE;
        if (!instance.isInited()) {
            instance.init();
        }
        return instance;
    }

    public boolean isInited() {
        return mRenderer != null;
    }
    public static void setContext(Context context) {
        copyResource(context);
    }

    public void init() {
        mRenderer = new GPUPixelRenderer();
        runOnDraw(new Runnable() {
            @Override
            public void run() {
                nativeContextInit();
            }
        });
    }

    public void destroy() {
        if (!isInited()) return;
        purge();
        setGLSurfaceView(null);
        setSource(null);
        mRenderer.clear();
        mRenderer = null;
    }

    public void setSource(GPUPixelSource source) {
        mRenderer.setSource(source);
    }

    public void setGLSurfaceView(final GLSurfaceView view) {
        mGLSurfaceView = view;
        if (mGLSurfaceView != null) {
            mGLSurfaceView.setEGLContextClientVersion(2);
            mGLSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
            mGLSurfaceView.getHolder().setFormat(PixelFormat.RGBA_8888);
            mGLSurfaceView.setRenderer(mRenderer);
            mGLSurfaceView.setRenderMode(mGLSurfaceViewRenderMode);
            if (Build.VERSION.SDK_INT > Build.VERSION_CODES.GINGERBREAD_MR1) {
                mGLSurfaceView.setPreserveEGLContextOnPause(true);
            }
            mGLSurfaceView.requestRender();
        } else {
            mGLSurfaceViewRenderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY;
        }
    }

    public void setGLSurfaceViewRenderMode(int glSurfaceViewRenderMode) {
        mGLSurfaceViewRenderMode = glSurfaceViewRenderMode;
        if (mGLSurfaceView != null) {
            mGLSurfaceView.setRenderMode(mGLSurfaceViewRenderMode);
        }
    }

    public GLSurfaceView getGLSurfaceView() {
        return mGLSurfaceView;
    }

    public void requestRender() {
        if (mGLSurfaceView != null) {
            mGLSurfaceView.requestRender();
        }
    }

    public void purge() {
        if (mGLSurfaceView != null) {
            GPUPixel.getInstance().runOnDraw(new Runnable() {
                @Override
                public void run() {
                    GPUPixel.nativeContextPurge();
                }
            });
            mGLSurfaceView.requestRender();
        } else {
            GPUPixel.nativeContextPurge();
        }
    }

    public GPUPixelRenderer getRenderer() {
        return mRenderer;
    }

    public boolean isPreDrawQueueEmpty() { return mRenderer.isPreDrawQueueEmpty(); }

    public void runOnPreDraw(Runnable runnable) {
        mRenderer.runOnPreDraw(runnable);
    }

    public boolean isDrawQueueEmpty() { return mRenderer.isDrawQueueEmpty(); }

    public void runOnDraw(Runnable runnable) {
        mRenderer.runOnDraw(runnable);
    }

    public boolean isPostDrawQueueEmpty() { return mRenderer.isPostDrawQueueEmpty(); }

    public void runOnPostDraw(Runnable runnable) {
        mRenderer.runOnPostDraw(runnable);
    }

    static {
        System.loadLibrary("gpupixel");
        System.loadLibrary("mars-face-kit");
    }

    public static void copyResource(Context context) {
        String exPath = context.getExternalFilesDir(null).getAbsolutePath();
        copyAssetsToFiles(context, "resource", exPath + "/resource");
        resource_path = exPath + "/resource";
    }

    public static String getResource_path() {
        return resource_path;
    }

    public static void  copyAssetsToFiles(Context context, String oldPath, String newPath) {
        try {
            String fileNames[] = context.getAssets().list(oldPath);//获取assets目录下的所有文件及目录名
            if (fileNames.length > 0) {//如果是目录
                File file = new File(newPath);
                if(!file.exists()) {
                    file.mkdirs();//如果文件夹不存在，则递归
                }
                for (String fileName : fileNames) {
                    String srcPath = oldPath + "/" + fileName;
                    String dstPath = newPath+"/"+fileName;
                    File f = new File(dstPath);
                    if(f.exists()) continue;
                    copyAssetsToFiles(context, srcPath, dstPath);
                }

            } else {//如果是文件
                File file = new File(newPath);
                if(!file.exists()) {
                    InputStream is = context.getAssets().open(oldPath);
                    FileOutputStream fos = new FileOutputStream(file);
                    byte[] buffer = new byte[1024];
                    int byteCount=0;
                    while((byteCount=is.read(buffer))!=-1) {//循环从输入流读取 buffer字节
                        fos.write(buffer, 0, byteCount);//将读取的输入流写入到输出流
                    }
                    fos.flush();//刷新缓冲区
                    is.close();
                    fos.close();
                }

            }
        } catch (Exception e) {
            // TODO Auto-generated catch block
            e.printStackTrace();

        }
    }

    // Filter
    public static native long nativeFilterCreate(final String filterClassName);
    public static native void nativeFilterDestroy(long classID);
    public static native void nativeFilterFinalize(long classID);
    public static native void nativeFilterSetPropertyFloat(long classID, String property, float value);
    public static native void nativeFilterSetPropertyInt(long classID, String property, int value);
    public static native void nativeFilterSetPropertyFloatArray(long classID, String property, float[] array);
    public static native void nativeFilterSetPropertyString(long classID, String prooerty, String value);
    // SourceImage
    public static native long nativeSourceImageNew();
    public static native void nativeSourceImageDestroy(final long classID);
    public static native void nativeSourceImageFinalize(final long classID);
    public static  native void nativeSourceImageSetImage(final long classID, final Bitmap bitmap);

    // SourceCamera
    public static native long nativeSourceCameraNew();
    public static native void nativeSourceCameraDestroy(final long classID);
    public static native void nativeSourceCameraFinalize(final long classID);
    public static native void nativeSourceCameraSetFrame(final long classID, final int width, final int height, final int[] data, final int rotation);

    // SourceRawDataInput
    public static native long nativeSourceRawInputNew();
    public static native void nativeSourceRawInputUploadBytes(final long classID, final int[] pixel, final int width, final int height, final int stride);
    public static native void nativeSourceRawInputSetRotation(final long classID, final int rotation);

    // Source
    public static native long nativeSourceAddTarget(final long classID, final long targetClassID, final int texID, final boolean isFilter);
    public static native void nativeSourceRemoveTarget(final long classID, final long targetClassID, final boolean isFilter);
    public static native void nativeSourceRemoveAllTargets(final long classID);
    public static native boolean nativeSourceProceed(final long classID, final boolean bUpdateTargets);
    public static native int nativeSourceGetRotatedFramebuferWidth(final long classID);
    public static native int nativeSourceGetRotatedFramebuferHeight(final long classID);
    public static native byte[] nativeSourceCaptureAProcessedFrameData(final long classId, final long upToFilterClassId, final int width, final int height);

    // view
    public static native long nativeTargetViewNew();
    public static native void nativeTargetViewFinalize(final long classID);
    public static native void nativeTargetViewOnSizeChanged(final long classID, final int width, final int height);
    public static native void nativeTargetViewSetFillMode(final long classID, final int fillMode);
    public static native void nativeTargetViewSetMirror(final long classID, final boolean mirror);
    // context
    public static native void nativeContextInit();
    public static native void nativeContextDestroy();
    public static native void nativeContextPurge();

    // utils
    public static native void nativeYUVtoRBGA(byte[] yuv, int width, int height, int[] out);

    public static native void nativeSetLandmarkCallback(Object source, final long classID);

}
