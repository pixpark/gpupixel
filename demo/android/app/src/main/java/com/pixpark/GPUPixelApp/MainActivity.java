package com.pixpark.GPUPixelApp;

import static android.widget.Toast.LENGTH_LONG;

import android.Manifest;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.SurfaceTexture;
import android.view.TextureView;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.Toast;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import com.pixpark.GPUPixelApp.databinding.ActivityMainBinding;
import com.pixpark.gpupixel.FaceDetector;
import com.pixpark.gpupixel.GPUPixel;
import com.pixpark.gpupixel.GPUPixelFilter;
import com.pixpark.gpupixel.GPUPixelSinkRawData;
import com.pixpark.gpupixel.GPUPixelSinkSurface;
import com.pixpark.gpupixel.GPUPixelSourceRawData;
import android.view.Surface;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

public class MainActivity extends AppCompatActivity {
    private static final int CAMERA_PERMISSION_REQUEST_CODE = 200;
    private static final String TAG = "GPUPixelDemo";

    Camera2Helper mCamera2Helper;
    private GPUPixelSourceRawData mSourceRawData;
    private GPUPixelFilter mBeautyFilter;
    private GPUPixelFilter mFaceReshapeFilter;
    private GPUPixelFilter mLipstickFilter;
    private FaceDetector mFaceDetector;
    private GPUPixelSinkSurface mSinkSurface;
    private GPUPixelSinkRawData mCaptureSinkRawData;

    private SeekBar mSmoothSeekbar;
    private SeekBar mWhitenessSeekbar;
    private SeekBar mThinFaceSeekbar;
    private SeekBar mBigeyeSeekbar;
    private SeekBar lipstickSeekbar;
    private TextureView mTextureView;

    private volatile boolean mCaptureRequested = false;
    private ExecutorService mCaptureExecutor;

    private ActivityMainBinding binding;
    //    private CainCameraWrapper cainCameraWrapper;
    // Memory buffer for taking pictures
    private ByteBuffer mTakePictureBuffer;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        long start = System.currentTimeMillis();
        // Get log path
        String path = getExternalFilesDir("gpupixel").getAbsolutePath();
        Log.i(TAG, "Log path: " + path);

        // Initialize GPUPixel
        GPUPixel.Init(this);
        mCaptureExecutor = Executors.newSingleThreadExecutor();

        // Asynchronously initialize face detector to avoid blocking main thread in setupCamera()
        mCaptureExecutor.execute(() -> {
            long fdStart = System.currentTimeMillis();
            mFaceDetector = FaceDetector.Create();
            Log.i(TAG, "FaceDetector init time: " + (System.currentTimeMillis() - fdStart));
        });

        // Keep screen on
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Initialize UI first (creates TextureView listener)
        initUI();

        // Setup camera (which creates mSinkSurface and connects to TextureView)
        // But wait for permission check first
        // Check camera permission
        checkCameraPermission();

        binding.btnSwitch.setOnClickListener(v -> {
            mCamera2Helper.switchCamera();
            // Update mirror setting after switching camera
            updateMirrorSetting();
        });

        binding.btnFlash.setOnClickListener(v -> {
            mCamera2Helper.toggleFlashlight();
        });

        binding.btnCapture.setOnClickListener(v -> requestCapture());
        Log.i(TAG, "MainActivity onCreate: " + (System.currentTimeMillis() - start));

    }

    /**
     * Initialize UI components
     * <p>
     * Main tasks:
     * 1. Get TextureView (used to display processed image)
     * 2. Set SurfaceTextureListener, pass Surface to SinkSurface when Surface is available
     */
    private void initUI() {
        // Try to get TextureView from binding, or find by ID
        mTextureView = binding.textureView;
        if (mTextureView == null) {
            // If there's no textureView in layout, try to find or create
            View rootView = binding.getRoot();
            mTextureView = rootView.findViewById(android.R.id.text1); // You may need to modify this ID
            if (mTextureView == null) {
                // If still can't find, output warning
                // You need to add a TextureView with id 'textureView' in layout XML
                Log.w(TAG, "TextureView not found in layout. Please add a TextureView with id 'textureView'");
            }
        }

        // Set SurfaceTexture listener
        // This is key: when TextureView's Surface is available, pass it to SinkSurface for rendering
        mTextureView.setSurfaceTextureListener(new TextureView.SurfaceTextureListener() {
            /**
             * Called when Surface is first available
             * At this time we can set Surface to SinkSurface, start rendering
             */
            @Override
            public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
                if (mSinkSurface != null) {
                    // Set Surface to SinkSurface, C layer will create EGL Window Surface
                    mSinkSurface.SetSurface(new Surface(surface), width, height);
                    Log.d(TAG, "TextureView surface available: " + width + "x" + height);
                }
            }

            /**
             * Called when Surface size changes
             * Need to re-set Surface (because size has changed)
             */
            @Override
            public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
                if (mSinkSurface != null) {
                    // Re-set Surface, will update EGL Surface size
                    mSinkSurface.SetSurface(new Surface(surface), width, height);
                    Log.d(TAG, "TextureView surface size changed: " + width + "x" + height);
                }
            }

            /**
             * Called when Surface is destroyed
             * Need to release Surface resources in SinkSurface
             */
            @Override
            public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
                if (mSinkSurface != null) {
                    // Release Surface resources, destroy EGL Window Surface
                    mSinkSurface.ReleaseSurface();
                }
                return false; // false means we release resources ourselves, don't need TextureView to delay destruction
            }

            /**
             * Called when Surface content is updated
             * No operations needed here, because rendering is controlled by SinkSurface in C layer
             */
            @Override
            public void onSurfaceTextureUpdated(SurfaceTexture surface) {
                // Texture updated, no operation needed (rendering controlled by C layer)
            }
        });

        // Setup beauty slider
        mSmoothSeekbar = binding.smoothSeekbar;
        mSmoothSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mBeautyFilter != null) {
                    mBeautyFilter.SetProperty("skin_smoothing", progress / 10.0f);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });

        mWhitenessSeekbar = binding.whitenessSeekbar;
        mWhitenessSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mBeautyFilter != null) {
                    mBeautyFilter.SetProperty("whiteness", progress / 10.0f);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });

        mThinFaceSeekbar = binding.thinfaceSeekbar;
        mThinFaceSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mFaceReshapeFilter != null) {
                    mFaceReshapeFilter.SetProperty("thin_face", progress / 160.0f);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
        mBigeyeSeekbar = binding.bigeyeSeekbar;
        mBigeyeSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mFaceReshapeFilter != null) {
                    mFaceReshapeFilter.SetProperty("big_eye", progress / 40.0f);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
        lipstickSeekbar = binding.lipstickSeekbar;
        lipstickSeekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                if (mLipstickFilter != null) {
                    mLipstickFilter.SetProperty("blend_level", progress / 10.0f);
                }
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {
            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {
            }
        });
    }

    /**
     * Setup camera and GPUPixel processing chain
     * <p>
     * This is the initialization method for the entire image processing pipeline:
     * 1. Create camera helper
     * 2. Create data source (Source)
     * 3. Create filter chain (Filters)
     * 4. Create output (SinkSurface) - directly render to TextureView
     * 5. Initialize face detection
     * 6. Set camera frame callback
     */
    private void setupCamera() {
        long start = System.currentTimeMillis();
        // Create and initialize camera (using Camera2 API)
        mCamera2Helper = new Camera2Helper(this);

        // Create data source for GPUPixel processing chain
        // SourceRawData is used to receive raw RGBA data
        mSourceRawData = GPUPixelSourceRawData.Create();

        // Create filters: lipstick filter -> beauty filter -> face reshaping filter
        mLipstickFilter = GPUPixelFilter.Create(GPUPixelFilter.LIPSTICK_FILTER);
        mBeautyFilter = GPUPixelFilter.Create(GPUPixelFilter.BEAUTY_FACE_FILTER);
        mFaceReshapeFilter = GPUPixelFilter.Create(GPUPixelFilter.FACE_RESHAPE_FILTER);

        // Create output - use SinkSurface for direct rendering
        // Note: must be created before setting camera callback
        mSinkSurface = GPUPixelSinkSurface.Create();
        mSinkSurface.SetFillMode(GPUPixelSinkSurface.PRESERVE_ASPECT_RATIO);

        // Mirror setting will be set after camera starts (in updateMirrorSetting)

        // Create raw data output Sink, used for saving processed RGBA data when taking photos
        mCaptureSinkRawData = GPUPixelSinkRawData.Create();

        // If TextureView is already available, set Surface immediately
        // Otherwise will be set in TextureView's SurfaceTextureListener callback
        if (mTextureView != null && mTextureView.isAvailable()) {
            SurfaceTexture surfaceTexture = mTextureView.getSurfaceTexture();
            if (surfaceTexture != null) {
                int width = mTextureView.getWidth();
                int height = mTextureView.getHeight();
                if (width > 0 && height > 0) {
                    Log.d(TAG, "setSurfaceTexture: " + width + "x" + height);
                    mSinkSurface.SetSurface(new Surface(surfaceTexture), width, height);
                }
            }
        }

        // Face detector initialized asynchronously in onCreate, not blocking here

        // Set camera frame callback
        // This callback will be called whenever camera generates a frame of data
        mCamera2Helper.setFrameCallback((rgbaData, width, height) -> {
            // Get camera sensor orientation (different devices have different camera sensor orientations)
            int sensorOrientation = mCamera2Helper.getSensorOrientation();

            // Check if it's front camera
            boolean isFrontCamera = GPUPixel.isFrontCamera(mCamera2Helper.getCameraFacing());

            // Use GPUPixel to calculate required image rotation angle
            // Considering device orientation, sensor orientation, and front/back camera
            int rotation = GPUPixel.calculateRotation(
                    MainActivity.this,
                    sensorOrientation,
                    isFrontCamera
            );

            // Use GPUPixel to rotate RGBA data (C layer implementation, better performance)
            byte[] rotatedData = GPUPixel.rotateRgbaImage(rgbaData, width, height, rotation);

            // After rotation width and height may be swapped (90 or 270 degree rotation)
            int outWidth = (rotation == 90 || rotation == 270) ? height : width;
            int outHeight = (rotation == 90 || rotation == 270) ? width : height;

            // Perform face detection (using rotated data) - skip if detector not ready
            float[] landmarks = null;
            if (mFaceDetector != null) {
                landmarks = mFaceDetector.detect(rotatedData, outWidth, outHeight,
                        outWidth * 4, FaceDetector.GPUPIXEL_MODE_FMT_VIDEO,
                        FaceDetector.GPUPIXEL_FRAME_TYPE_RGBA);
            }

            // If face detected, set landmark coordinates to filters
            if (landmarks != null && landmarks.length > 0) {
                mFaceReshapeFilter.SetProperty("face_landmark", landmarks);
                mLipstickFilter.SetProperty("face_landmark", landmarks);
            }

            // Pass rotated RGBA data into GPUPixel processing chain
            // Processing flow: Source -> lipstick filter -> beauty filter -> face reshaping filter -> SinkSurface
            // SinkSurface will render directly in C layer EGL environment to TextureView, no Java layer intervention needed
            mSourceRawData.ProcessData(
                    rotatedData,
                    outWidth,
                    outHeight,
                    outWidth * 4,  // Number of bytes per row (width * 4 bytes RGBA)
                    GPUPixelSourceRawData.FRAME_TYPE_RGBA
            );

            // Note: No need to get processed RGBA data, no need for manual rendering
            // SinkSurface will automatically complete rendering in C layer and swap buffers to display to screen

            if (mCaptureRequested && mCaptureSinkRawData != null) {
                mCaptureRequested = false;
                byte[] captureRgba = mCaptureSinkRawData.GetRgbaBuffer();
                int captureWidth = mCaptureSinkRawData.GetWidth();
                int captureHeight = mCaptureSinkRawData.GetHeight();
                if (captureRgba != null
                        && captureWidth > 0
                        && captureHeight > 0
                        && captureRgba.length >= captureWidth * captureHeight * 4) {
//                    byte[] bufferCopy = captureRgba.clone();
                    mCaptureExecutor.execute(
                            () -> saveCapturedImage(captureRgba, captureWidth, captureHeight));
                } else {
                    runOnUiThread(() ->
                            Toast.makeText(MainActivity.this, "Photo capture failed, invalid data", Toast.LENGTH_SHORT)
                                    .show());
                }
            }
        });

        // Connect processing chain: data source -> filter1 -> filter2 -> filter3 -> output
        // Data flow: SourceRawData -> LipstickFilter -> BeautyFilter -> FaceReshapeFilter -> SinkSurface
        mSourceRawData.AddSink(mLipstickFilter);
        mLipstickFilter.AddSink(mBeautyFilter);
        mBeautyFilter.AddSink(mFaceReshapeFilter);
        mFaceReshapeFilter.AddSink(mSinkSurface);
        if (mCaptureSinkRawData != null) {
            mFaceReshapeFilter.AddSink(mCaptureSinkRawData);
        }

        // Start camera, begin capturing images
        mCamera2Helper.startCamera();

        // Set initial mirror setting after camera is ready
        // This will be called again when switching cameras
        updateMirrorSetting();

        Log.d(TAG, "setupCamera: cost " + (System.currentTimeMillis() - start));
    }

    /**
     * Update mirror setting
     * <p>
     * Update SinkSurface mirror status based on current camera type and mirror setting.
     * Front camera defaults to enabled mirror (like looking in a mirror), back camera does not mirror.
     */
    private void updateMirrorSetting() {
        if (mSinkSurface != null && mCamera2Helper != null) {
            boolean shouldMirror = mCamera2Helper.shouldMirrorPreview();
            mSinkSurface.SetMirror(shouldMirror);
            Log.d(TAG, "Mirror setting updated: " + shouldMirror +
                    " (Camera: " + (mCamera2Helper.getCameraFacing() ==
                    android.hardware.camera2.CameraCharacteristics.LENS_FACING_FRONT ? "FRONT" : "BACK") +
                    ", MirrorEnabled: " + mCamera2Helper.isMirrorEnabled() + ")");
        }
    }

    /**
     * Request capture, mark to save after next frame processing is completed
     */
    private void requestCapture() {
        if (mCaptureSinkRawData == null) {
            Toast.makeText(this, "Photo capture function not initialized", Toast.LENGTH_SHORT).show();
            return;
        }
        if (mCaptureExecutor == null || mCaptureExecutor.isShutdown()) {
            Toast.makeText(this, "Save thread not available", Toast.LENGTH_SHORT).show();
            return;
        }
        if (mCaptureRequested) {
            Toast.makeText(this, "Saving in progress, please wait", Toast.LENGTH_SHORT).show();
            return;
        }
        mCaptureRequested = true;
        Toast.makeText(this, "Saving current image", Toast.LENGTH_SHORT).show();
    }

    /**
     * Save RGBA data as PNG file to app cache directory
     */
    private void saveCapturedImage(byte[] rgbaData, int width, int height) {
        int pixelCount = width * height * 4;
        // if size changed, clear and reallocate
        if (mTakePictureBuffer != null && mTakePictureBuffer.capacity() != pixelCount) {
            mTakePictureBuffer.clear();
            mTakePictureBuffer = null;
        }
        if (mTakePictureBuffer == null) {
            mTakePictureBuffer = ByteBuffer.allocateDirect(pixelCount);
        }
        mTakePictureBuffer.rewind();
        mTakePictureBuffer.put(rgbaData);
        mTakePictureBuffer.position(0);
        Bitmap bitmap = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
        bitmap.copyPixelsFromBuffer(mTakePictureBuffer);

        File captureDir = new File(getCacheDir(), "captures");
        if (!captureDir.exists() && !captureDir.mkdirs()) {
            runOnUiThread(() ->
                    Toast.makeText(MainActivity.this, "Failed to create cache directory", Toast.LENGTH_SHORT).show());
            bitmap.recycle();
            return;
        }

        String timeStamp = new SimpleDateFormat("yyyyMMdd_HHmmss", Locale.US).format(new Date());
        File outFile = new File(captureDir, "gpupixel_" + timeStamp + ".png");

        try (FileOutputStream fos = new FileOutputStream(outFile)) {
            bitmap.compress(Bitmap.CompressFormat.PNG, 100, fos);
            fos.flush();
            runOnUiThread(() ->
                    Toast.makeText(MainActivity.this,
                            "Save successful: " + outFile.getAbsolutePath(), Toast.LENGTH_SHORT).show());
        } catch (IOException e) {
            Log.e(TAG, "Failed to save capture", e);
            runOnUiThread(() ->
                    Toast.makeText(MainActivity.this,
                            "Save failed: " + e.getMessage(), Toast.LENGTH_LONG).show());
        } finally {
            bitmap.recycle();
        }
    }

    public void checkCameraPermission() {
        // Check camera permission
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            // If no camera permission, request permission
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA},
                    CAMERA_PERMISSION_REQUEST_CODE);
        } else {
            // Has permission, set up camera
            setupCamera();
        }
    }

    @Override
    public void onRequestPermissionsResult(
            int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == CAMERA_PERMISSION_REQUEST_CODE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                setupCamera();
            } else {
                Toast.makeText(this, "No camera permission!", LENGTH_LONG).show();
            }
        }
    }

    @Override
    protected void onResume() {
        super.onResume();

        if (mCamera2Helper != null && !mCamera2Helper.isCameraOpened()) {
            mCamera2Helper.startCamera();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();

        if (mCamera2Helper != null) {
            mCamera2Helper.stopCamera();
        }
    }

    @Override
    protected void onDestroy() {
        // Release camera resources
        if (mCamera2Helper != null) {
            mCamera2Helper.stopCamera();
            mCamera2Helper = null;
        }

        // Release face detector
        if (mFaceDetector != null) {
            mFaceDetector.destroy();
            mFaceDetector = null;
        }
        //
        // Release GPUPixel resources
        if (mBeautyFilter != null) {
            mBeautyFilter.Destroy();
            mBeautyFilter = null;
        }
        if (mFaceReshapeFilter != null) {
            mFaceReshapeFilter.Destroy();
            mFaceReshapeFilter = null;
        }

        // Release GPUPixel resources
        if (mLipstickFilter != null) {
            mLipstickFilter.Destroy();
            mLipstickFilter = null;
        }

        if (mSourceRawData != null) {
            mSourceRawData.Destroy();
            mSourceRawData = null;
        }

        if (mCaptureSinkRawData != null) {
            mCaptureSinkRawData.Destroy();
            mCaptureSinkRawData = null;
        }

        if (mSinkSurface != null) {
            mSinkSurface.Destroy();
            mSinkSurface = null;
        }

        if (mCaptureExecutor != null) {
            mCaptureExecutor.shutdownNow();
            mCaptureExecutor = null;
        }
        super.onDestroy();
    }
}
