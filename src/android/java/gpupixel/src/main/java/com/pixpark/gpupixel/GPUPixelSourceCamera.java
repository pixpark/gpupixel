/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.annotation.TargetApi;
import android.content.Context;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.Camera;
import android.opengl.GLES20;
import android.os.Build;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class GPUPixelSourceCamera extends GPUPixelSource implements Camera.PreviewCallback {
    private Camera mCamera;
    private int mCurrentCameraId = 1;
    private ByteBuffer mByteBuffer;
    private int mRotation = GPUPixel.NoRotation;
    private Context mContext;
    private SurfaceTexture mSurfaceTexture = null;
    private GPUPixelSourceRawData SourceRawData = null;
    private Object object_this;
    
    // 添加数据回调接口
    public interface FrameDataCallback {
        void onFrameData(ByteBuffer rgbaData, int width, int height);
    }
    
    private FrameDataCallback mFrameDataCallback = null;
    
    public GPUPixelSourceCamera(Context context) {
        mContext = context;
        object_this = this;
        if (mNativeClassID != 0) return;
        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeSourceCameraNew();
            }
        });

        setUpCamera(mCurrentCameraId);
    }
    
    // 设置数据回调
    public void setFrameDataCallback(FrameDataCallback callback) {
        mFrameDataCallback = callback;
    }

    @Override
    public void onPreviewFrame(final byte[] data, Camera camera) {
        final Camera.Size previewSize = camera.getParameters().getPreviewSize();
        final int frameSize = previewSize.width * previewSize.height;
        
        if (mByteBuffer == null) {
            // 初始化ByteBuffer用于存储RGBA数据
            mByteBuffer = ByteBuffer.allocateDirect(frameSize * 4);
            mByteBuffer.order(ByteOrder.nativeOrder());
        }
        final Camera cam = camera;

        GPUPixel.GetInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    // 直接从YUV转换为RGBA的byte[]
                    mByteBuffer.clear();
                    GPUPixel.nativeYUVtoRBGA(data, previewSize.width, previewSize.height, mByteBuffer.array());
                    mByteBuffer.position(frameSize * 4); // 设置位置到末尾
                    mByteBuffer.flip(); // 准备读取
                    
                    // 通过回调接口将RGBA数据回调出去
                    if (mFrameDataCallback != null) {
                        mFrameDataCallback.onFrameData(mByteBuffer, previewSize.height, previewSize.width);
                        mByteBuffer.rewind(); // 重置以便后续使用
                    }
                    
                    cam.addCallbackBuffer(data);
                    GPUPixel.nativeSourceCameraSetFrame(mNativeClassID, previewSize.height, previewSize.width, mByteBuffer.array(), GPUPixel.NoRotation);
                }
            }
        });
        DoRender(true, true);
    }

    public void onResume() {
        setUpCamera(mCurrentCameraId);
    }

    public void onPause() {
        releaseCamera();
    }

    public void switchCamera() {
        releaseCamera();
        mCurrentCameraId = (mCurrentCameraId + 1) % Camera.getNumberOfCameras();
        setUpCamera(mCurrentCameraId);
    }

    public void SetSourceRawInput(final GPUPixelSourceRawData source_input) {
        SourceRawData = source_input;
    }

    private void setUpCamera(final int id) {
        mCamera = Camera.open(id);
        Camera.Parameters parameters = mCamera.getParameters();
        if (parameters.getSupportedFocusModes().contains(
                Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE)) {
            parameters.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_PICTURE);
        }
        parameters.setPreviewSize(1280, 720);
        parameters.setPreviewFormat(ImageFormat.NV21);
        mCamera.setParameters(parameters);

        int deviceRotation = ((WindowManager) mContext.getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay()
                .getRotation();
        android.hardware.Camera.CameraInfo info = new android.hardware.Camera.CameraInfo();
        android.hardware.Camera.getCameraInfo(mCurrentCameraId, info);

        int rotation = 0;
        switch (deviceRotation) {
            case Surface.ROTATION_0:
                rotation = 0;
                break;
            case Surface.ROTATION_90:
                rotation = 90;
                break;
            case Surface.ROTATION_180:
                rotation = 180;
                break;
            case Surface.ROTATION_270:
                rotation = 270;
                break;
        }

        mRotation = GPUPixel.NoRotation;
        if (info.facing == Camera.CameraInfo.CAMERA_FACING_FRONT) {
            rotation = (info.orientation + rotation) % 360;
            switch (rotation) {
                case 0:
                    mRotation = GPUPixel.FlipHorizontal;
                    break;
                case 90:
                    mRotation = GPUPixel.RotateRightFlipVertical;
                    break;
                case 180:
                    mRotation = GPUPixel.FlipVertical;
                    break;
                case 270:
                    mRotation = GPUPixel.RotateRightFlipHorizontal;
                    break;
            }
        } else {
            rotation = (info.orientation - rotation + 360) % 360;
            switch (rotation) {
                case 90:
                    mRotation = GPUPixel.RotateRight;
                    break;
                case 180:
                    mRotation = GPUPixel.Rotate180;
                    break;
                case 270:
                    mRotation = GPUPixel.RotateLeft;
                    break;
            }
        }

        if (Build.VERSION.SDK_INT > Build.VERSION_CODES.GINGERBREAD_MR1) {
            GPUPixel.GetInstance().runOnDraw(new Runnable() {
                @TargetApi(Build.VERSION_CODES.HONEYCOMB)
                @Override
                public void run() {
                    if (mNativeClassID != 0) {
                        int[] textures = new int[1];
                        GLES20.glGenTextures(1, textures, 0);
                        mSurfaceTexture = new SurfaceTexture(textures[0]);
                        try {
                            mCamera.setPreviewTexture(mSurfaceTexture);
                            mCamera.setPreviewCallback(GPUPixelSourceCamera.this);
                            mCamera.startPreview();
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }
            });
            GPUPixel.GetInstance().requestRender();
        } else {
            mCamera.setPreviewCallback(this);
            mCamera.startPreview();
        }
    }
    public void setPreviewHolder(SurfaceHolder holder) {
        try {
            mCamera.setPreviewDisplay(holder);
        } catch (IOException e) {
            e.printStackTrace();
        }
        mCamera.startPreview();
    };


    private void releaseCamera() {
        mSurfaceTexture = null;
        mCamera.setPreviewCallback(null);
        mCamera.release();
        mCamera = null;
    }

    public void Destroy() {
        Destroy(true);
    }

    public void Destroy(boolean onGLThread) {
        if (mNativeClassID != 0) {
            if (onGLThread) {
                GPUPixel.GetInstance().runOnDraw(new Runnable() {
                    @Override
                    public void run() {
                        if (mNativeClassID != 0) {
                            GPUPixel.nativeSourceCameraDestroy(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    }
                });
            } else {
                GPUPixel.nativeSourceCameraDestroy(mNativeClassID);
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
                            GPUPixel.nativeSourceCameraFinalize(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    });
                    GPUPixel.GetInstance().requestRender();
                } else {
                    GPUPixel.nativeSourceCameraFinalize(mNativeClassID);
                    mNativeClassID = 0;
                }
            }
        } finally {
            super.finalize();
        }
    }
}
