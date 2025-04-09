package com.pixpark.GPUPixelApp;

import android.Manifest;
import android.annotation.SuppressLint;
import android.content.Context;
import android.content.pm.PackageManager;
import android.graphics.ImageFormat;
import android.graphics.SurfaceTexture;
import android.hardware.camera2.CameraAccessException;
import android.hardware.camera2.CameraCaptureSession;
import android.hardware.camera2.CameraCharacteristics;
import android.hardware.camera2.CameraDevice;
import android.hardware.camera2.CameraManager;
import android.hardware.camera2.CaptureRequest;
import android.hardware.camera2.params.StreamConfigurationMap;
import android.media.Image;
import android.media.ImageReader;
import android.os.Handler;
import android.os.HandlerThread;
import android.util.Log;
import android.util.Size;
import android.view.Surface;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;

import java.nio.ByteBuffer;
import java.util.Arrays;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;
import java.util.concurrent.Semaphore;
import java.util.concurrent.TimeUnit;

import com.pixpark.gpupixel.GPUPixel;

public class Camera2Helper {
    private static final String TAG = "Camera2Helper";
    public static final int CAMERA_FACING =
            CameraCharacteristics.LENS_FACING_FRONT; // Default to front camera

    private Context mContext;
    private CameraDevice mCameraDevice;
    private CameraCaptureSession mCaptureSession;
    private ImageReader mImageReader;
    private Size mPreviewSize;
    private HandlerThread mBackgroundThread;
    private Handler mBackgroundHandler;
    private Semaphore mCameraOpenCloseLock = new Semaphore(1);
    private int mSensorOrientation;
    private FrameCallback mFrameCallback;

    private boolean mIsCameraOpened = false;

    public interface FrameCallback {
        void onFrameAvailable(byte[] rgbaData, int width, int height);
    }

    public Camera2Helper(Context context) {
        mContext = context;
    }

    public void setFrameCallback(FrameCallback callback) {
        mFrameCallback = callback;
    }

    public void startCamera() {
        startBackgroundThread();
        openCamera();
    }

    public void stopCamera() {
        closeCamera();
        stopBackgroundThread();
    }

    private void startBackgroundThread() {
        mBackgroundThread = new HandlerThread("CameraBackground");
        mBackgroundThread.start();
        mBackgroundHandler = new Handler(mBackgroundThread.getLooper());
    }

    private void stopBackgroundThread() {
        if (mBackgroundThread != null) {
            mBackgroundThread.quitSafely();
            try {
                mBackgroundThread.join();
                mBackgroundThread = null;
                mBackgroundHandler = null;
            } catch (InterruptedException e) {
                Log.e(TAG, "Interrupted when stopping background thread", e);
            }
        }
    }

    @SuppressLint("MissingPermission")
    private void openCamera() {
        if (ActivityCompat.checkSelfPermission(mContext, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            Log.e(TAG, "No camera permission");
            return;
        }

        CameraManager manager = (CameraManager) mContext.getSystemService(Context.CAMERA_SERVICE);
        try {
            String cameraId = getCameraId(manager);
            if (cameraId == null) {
                Log.e(TAG, "Failed to find appropriate camera");
                return;
            }

            // Get camera characteristics
            CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);
            StreamConfigurationMap map =
                    characteristics.get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP);
            if (map == null) {
                Log.e(TAG, "Cannot get available preview sizes");
                return;
            }

            // Get sensor orientation
            mSensorOrientation = characteristics.get(CameraCharacteristics.SENSOR_ORIENTATION);

            // Choose optimal preview size
            mPreviewSize = chooseOptimalSize(map.getOutputSizes(SurfaceTexture.class), 1280, 720);

            // Set up ImageReader to handle preview frames
            mImageReader = ImageReader.newInstance(
                    mPreviewSize.getWidth(), mPreviewSize.getHeight(), ImageFormat.YUV_420_888, 2);
            mImageReader.setOnImageAvailableListener(mOnImageAvailableListener, mBackgroundHandler);

            // Open camera
            if (!mCameraOpenCloseLock.tryAcquire(2500, TimeUnit.MILLISECONDS)) {
                throw new RuntimeException("Time out waiting to lock camera opening.");
            }

            manager.openCamera(cameraId, mStateCallback, mBackgroundHandler);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed to access camera", e);
        } catch (InterruptedException e) {
            Log.e(TAG, "Interrupted while trying to lock camera opening", e);
        }
    }

    private String getCameraId(CameraManager manager) {
        try {
            for (String cameraId : manager.getCameraIdList()) {
                CameraCharacteristics characteristics = manager.getCameraCharacteristics(cameraId);
                Integer facing = characteristics.get(CameraCharacteristics.LENS_FACING);
                if (facing != null && facing == CAMERA_FACING) {
                    return cameraId;
                }
            }
            // If front camera not found, try to use any available camera
            if (manager.getCameraIdList().length > 0) {
                return manager.getCameraIdList()[0];
            }
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed to get camera ID", e);
        }
        return null;
    }

    private Size chooseOptimalSize(Size[] choices, int width, int height) {
        List<Size> bigEnough = Arrays.asList(choices);

        // Sort by area
        Collections.sort(bigEnough, new Comparator<Size>() {
            @Override
            public int compare(Size lhs, Size rhs) {
                // Sort in descending order
                return Long.signum((long) rhs.getWidth() * rhs.getHeight()
                        - (long) lhs.getWidth() * lhs.getHeight());
            }
        });

        // Choose one that doesn't exceed target resolution and is large enough
        for (Size option : bigEnough) {
            if (option.getWidth() <= width && option.getHeight() <= height) {
                return option;
            }
        }

        // If no suitable size found, choose the smallest one
        return bigEnough.get(bigEnough.size() - 1);
    }

    private final CameraDevice.StateCallback mStateCallback = new CameraDevice.StateCallback() {
        @Override
        public void onOpened(@NonNull CameraDevice cameraDevice) {
            mCameraOpenCloseLock.release();
            mCameraDevice = cameraDevice;
            createCaptureSession();
            mIsCameraOpened = true;
        }

        @Override
        public void onDisconnected(@NonNull CameraDevice cameraDevice) {
            mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
            mIsCameraOpened = false;
        }

        @Override
        public void onError(@NonNull CameraDevice cameraDevice, int error) {
            mCameraOpenCloseLock.release();
            cameraDevice.close();
            mCameraDevice = null;
            mIsCameraOpened = false;
            Log.e(TAG, "Camera device error: " + error);
        }
    };

    private void createCaptureSession() {
        try {
            if (mCameraDevice == null || mImageReader == null) return;

            // Create session
            Surface surface = mImageReader.getSurface();

            final CaptureRequest.Builder builder =
                    mCameraDevice.createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW);
            builder.addTarget(surface);

            mCameraDevice.createCaptureSession(
                    Collections.singletonList(surface), new CameraCaptureSession.StateCallback() {
                        @Override
                        public void onConfigured(
                                @NonNull CameraCaptureSession cameraCaptureSession) {
                            if (mCameraDevice == null) return;

                            mCaptureSession = cameraCaptureSession;
                            try {
                                // Auto focus
                                builder.set(CaptureRequest.CONTROL_AF_MODE,
                                        CaptureRequest.CONTROL_AF_MODE_CONTINUOUS_PICTURE);
                                // Auto exposure
                                builder.set(CaptureRequest.CONTROL_AE_MODE,
                                        CaptureRequest.CONTROL_AE_MODE_ON_AUTO_FLASH);

                                // Start preview
                                CaptureRequest request = builder.build();
                                mCaptureSession.setRepeatingRequest(
                                        request, null, mBackgroundHandler);
                            } catch (CameraAccessException e) {
                                Log.e(TAG, "Failed to set up capture request", e);
                            }
                        }

                        @Override
                        public void onConfigureFailed(
                                @NonNull CameraCaptureSession cameraCaptureSession) {
                            Log.e(TAG, "Failed to configure capture session");
                        }
                    }, mBackgroundHandler);
        } catch (CameraAccessException e) {
            Log.e(TAG, "Failed to create capture session", e);
        }
    }

    private void closeCamera() {
        try {
            mCameraOpenCloseLock.acquire();

            if (mCaptureSession != null) {
                mCaptureSession.close();
                mCaptureSession = null;
            }

            if (mCameraDevice != null) {
                mCameraDevice.close();
                mCameraDevice = null;
            }

            if (mImageReader != null) {
                mImageReader.close();
                mImageReader = null;
            }

            mIsCameraOpened = false;
        } catch (InterruptedException e) {
            Log.e(TAG, "Interrupted while trying to lock camera closing", e);
        } finally {
            mCameraOpenCloseLock.release();
        }
    }

    private final ImageReader.OnImageAvailableListener mOnImageAvailableListener =
            new ImageReader.OnImageAvailableListener() {
                @Override
                public void onImageAvailable(ImageReader reader) {
                    Image image = null;
                    try {
                        image = reader.acquireLatestImage();
                        if (image == null) return;

                        if (mFrameCallback != null) {
                            // Use GPUPixel for format conversion
                            byte[] rgbaData = GPUPixel.YUV_420_888toRGBA(image);
                            mFrameCallback.onFrameAvailable(
                                    rgbaData, image.getWidth(), image.getHeight());
                        }
                    } catch (Exception e) {
                        Log.e(TAG, "Failed to process image", e);
                    } finally {
                        if (image != null) {
                            image.close();
                        }
                    }
                }
            };

    public boolean isCameraOpened() {
        return mIsCameraOpened;
    }

    public Size getPreviewSize() {
        return mPreviewSize;
    }

    public int getSensorOrientation() {
        return mSensorOrientation;
    }
}
