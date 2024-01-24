/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
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
import android.util.Log;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import java.io.IOException;
import java.nio.IntBuffer;
import com.pixpark.gpupixel.GPUPixelSourceRawInput;

public class GPUPixelSourceCamera extends GPUPixelSource implements Camera.PreviewCallback {
    private Camera mCamera;
    private int mCurrentCameraId = 1;
    private IntBuffer mRGBABuffer;
    private int mRotation = GPUPixel.NoRotation;
    private Context mContext;
    private SurfaceTexture mSurfaceTexture = null;
    private GPUPixelSourceRawInput SourceRawDataInput = null;

    public GPUPixelSourceCamera(Context context) {
        mContext = context;
        if (mNativeClassID != 0) return;
        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                mNativeClassID = GPUPixel.nativeSourceCameraNew();
            }
        });

        setUpCamera(mCurrentCameraId);
    }

    @Override
    public void onPreviewFrame(final byte[] data, Camera camera) {
        final Camera.Size previewSize = camera.getParameters().getPreviewSize();
        if (mRGBABuffer == null) {
            mRGBABuffer = IntBuffer.allocate(previewSize.width * previewSize.height);
        }
        final Camera cam = camera;

        GPUPixel.getInstance().runOnDraw(new Runnable() {
            @Override
            public void run() {
                if (mNativeClassID != 0) {
                    // todo(jeayo yuv to texture)
                    GPUPixel.nativeYUVtoRBGA(data, previewSize.width, previewSize.height, mRGBABuffer.array());
                    cam.addCallbackBuffer(data);
                    GPUPixel.nativeSourceCameraSetFrame(mNativeClassID, previewSize.height, previewSize.width, mRGBABuffer.array(), GPUPixel.NoRotation);
                }
            }
        });
        proceed(true, true);
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

    public void SetSourceRawInput(final GPUPixelSourceRawInput source_input) {
        SourceRawDataInput = source_input;
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
            GPUPixel.getInstance().runOnDraw(new Runnable() {
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
            GPUPixel.getInstance().requestRender();
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

    public void destroy() {
        destroy(true);
    }

    public void destroy(boolean onGLThread) {
        if (mNativeClassID != 0) {
            if (onGLThread) {
                GPUPixel.getInstance().runOnDraw(new Runnable() {
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
                if (GPUPixel.getInstance().getGLSurfaceView() != null) {
                    GPUPixel.getInstance().runOnDraw(new Runnable() {
                        @Override
                        public void run() {
                            GPUPixel.nativeSourceCameraFinalize(mNativeClassID);
                            mNativeClassID = 0;
                        }
                    });
                    GPUPixel.getInstance().requestRender();
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
