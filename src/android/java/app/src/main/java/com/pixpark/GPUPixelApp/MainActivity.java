package com.pixpark.GPUPixelApp;

import static android.widget.Toast.LENGTH_LONG;

import android.Manifest;
import android.content.pm.PackageManager;
import android.hardware.camera2.CameraCharacteristics;
import android.opengl.GLES20;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.util.Log;
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
import com.pixpark.gpupixel.GPUPixelSourceRawData;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.FloatBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class MainActivity extends AppCompatActivity {
    private static final int CAMERA_PERMISSION_REQUEST_CODE = 200;
    private static final String TAG = "GPUPixelDemo";

    private Camera2Helper mCamera2Helper;
    private GPUPixelSourceRawData mSourceRawData;
    private GPUPixelFilter mBeautyFilter;
    private GPUPixelFilter mFaceReshapeFilter;
    private GPUPixelFilter mLipstickFilter;
    private FaceDetector mFaceDetector;
    private GPUPixelSinkRawData mSinkRawData;

    private SeekBar mSmoothSeekbar;
    private SeekBar mWhitenessSeekbar;
    private SeekBar mThinFaceSeekbar;
    private SeekBar mBigeyeSeekbar;
    private SeekBar lipstickSeekbar;
    private GLSurfaceView mGLSurfaceView;
    private MyRenderer mRenderer;

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // Get log path
        String path = getExternalFilesDir("gpupixel").getAbsolutePath();
        Log.i(TAG, "Log path: " + path);

        // Initialize GPUPixel
        GPUPixel.Init(this);

        // Keep screen on
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // Initialize UI
        initUI();

        // Check camera permission
        checkCameraPermission();
    }

    private void initUI() {
        mGLSurfaceView = binding.surfaceView;
        mGLSurfaceView.setEGLContextClientVersion(2);
        mRenderer = new MyRenderer();
        mGLSurfaceView.setRenderer(mRenderer);

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
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
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
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
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
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
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
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
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
            public void onStartTrackingTouch(SeekBar seekBar) {}

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {}
        });
    }

    private void setupCamera() {
        // Create and initialize camera
        mCamera2Helper = new Camera2Helper(this);

        // Create GPUPixel processing chain
        mSourceRawData = GPUPixelSourceRawData.Create();

        // Create filters
        mBeautyFilter = GPUPixelFilter.Create(GPUPixelFilter.BEAUTY_FACE_FILTER);
        mFaceReshapeFilter = GPUPixelFilter.Create(GPUPixelFilter.FACE_RESHAPE_FILTER);
        mLipstickFilter = GPUPixelFilter.Create(GPUPixelFilter.LIPSTICK_FILTER);

        // Create output sink
        mSinkRawData = GPUPixelSinkRawData.Create();

        // Initialize face detection
        mFaceDetector = FaceDetector.Create();

        // Set camera frame callback
        mCamera2Helper.setFrameCallback(new Camera2Helper.FrameCallback() {
            @Override
            public void onFrameAvailable(byte[] rgbaData, int width, int height) {
                // Get sensor orientation
                int sensorOrientation = mCamera2Helper.getSensorOrientation();

                // Check if front camera
                boolean isFrontCamera = GPUPixel.isFrontCamera(Camera2Helper.CAMERA_FACING);

                // Calculate rotation using GPUPixel
                int rotation = GPUPixel.calculateRotation(
                        MainActivity.this, sensorOrientation, isFrontCamera);

                // Rotate RGBA data using GPUPixel
                byte[] rotatedData =
                        GPUPixel.rotateRgbaImage(rgbaData, width, height, rotation);

                // Width and height may be swapped after rotation
                int outWidth = (rotation == 90 || rotation == 270) ? height : width;
                int outHeight = (rotation == 90 || rotation == 270) ? width : height;

                // Perform face detection (using rotated data)
                float[] landmarks = mFaceDetector.detect(rotatedData, outWidth, outHeight,
                        outWidth * 4, FaceDetector.GPUPIXEL_MODE_FMT_VIDEO,
                        FaceDetector.GPUPIXEL_FRAME_TYPE_RGBA);

                if (landmarks != null && landmarks.length > 0) {
                    Log.d(TAG, "Face landmarks detected: " + landmarks.length);
                    mFaceReshapeFilter.SetProperty("face_landmark", landmarks);
                    mLipstickFilter.SetProperty("face_landmark", landmarks);
                }

                // Process the rotated RGBA data with GPUPixelSourceRawData
                mSourceRawData.ProcessData(rotatedData, outWidth, outHeight, outWidth * 4,
                        GPUPixelSourceRawData.FRAME_TYPE_RGBA);

                // Get processed RGBA data
                byte[] processedRgba = mSinkRawData.GetRgbaBuffer();

                // Set texture data and request redraw
                if (processedRgba != null && mRenderer != null) {
                    int rgbaWidth = mSinkRawData.GetWidth();
                    int rgbaHeight = mSinkRawData.GetHeight();

                    mRenderer.updateTextureData(processedRgba, rgbaWidth, rgbaHeight, 0);

                    // Request GLSurfaceView to redraw
                    mGLSurfaceView.requestRender();
                }
            }
        });

        mSourceRawData.AddSink(mLipstickFilter);
        mLipstickFilter.AddSink(mBeautyFilter);
        mBeautyFilter.AddSink(mFaceReshapeFilter);
        mFaceReshapeFilter.AddSink(mSinkRawData);

        // Start camera
        mCamera2Helper.startCamera();
    }

    // OpenGL Renderer class
    private class MyRenderer implements GLSurfaceView.Renderer {
        private int mTextureID = -1;
        private int mProgramHandle;
        private int mPositionHandle;
        private int mTextureCoordHandle;
        private int mTextureSamplerHandle;

        private ByteBuffer mTextureData;
        private int mTextureWidth;
        private int mTextureHeight;
        private int mRotation = 0; // Image rotation angle
        private boolean mTextureNeedsUpdate = false;
        private final Object mLock = new Object();

        // Vertex coordinates
        private final float[] VERTICES = {
                -1.0f, -1.0f, // Bottom left
                1.0f, -1.0f, // Bottom right
                -1.0f, 1.0f, // Top left
                1.0f, 1.0f // Top right
        };

        // Texture coordinates - default (0° rotation)
        private final float[] TEXTURE_COORDS_0 = {
                0.0f, 1.0f, // Bottom left
                1.0f, 1.0f, // Bottom right
                0.0f, 0.0f, // Top left
                1.0f, 0.0f // Top right
        };

        // Texture coordinates - 90° clockwise rotation
        private final float[] TEXTURE_COORDS_90 = {
                0.0f, 0.0f, // Bottom left
                0.0f, 1.0f, // Bottom right
                1.0f, 0.0f, // Top left
                1.0f, 1.0f // Top right
        };

        // Texture coordinates - 180° clockwise rotation
        private final float[] TEXTURE_COORDS_180 = {
                1.0f, 0.0f, // Bottom left
                0.0f, 0.0f, // Bottom right
                1.0f, 1.0f, // Top left
                0.0f, 1.0f // Top right
        };

        // Texture coordinates - 270° clockwise rotation
        private final float[] TEXTURE_COORDS_270 = {
                1.0f, 1.0f, // Bottom left
                1.0f, 0.0f, // Bottom right
                0.0f, 1.0f, // Top left
                0.0f, 0.0f // Top right
        };

        // Texture coordinates for front camera mirroring - default (0° rotation)
        private final float[] TEXTURE_COORDS_MIRROR_0 = {
                1.0f, 1.0f, // Bottom left
                0.0f, 1.0f, // Bottom right
                1.0f, 0.0f, // Top left
                0.0f, 0.0f // Top right
        };

        // Currently used texture coordinates
        private float[] TEXTURE_COORDS = TEXTURE_COORDS_0;

        private FloatBuffer mVertexBuffer;
        private FloatBuffer mTexCoordBuffer;

        // View dimension properties
        private int mViewWidth;
        private int mViewHeight;

        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            // Set background color
            GLES20.glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

            // Initialize vertex coordinate buffer
            ByteBuffer bb = ByteBuffer.allocateDirect(VERTICES.length * 4);
            bb.order(ByteOrder.nativeOrder());
            mVertexBuffer = bb.asFloatBuffer();
            mVertexBuffer.put(VERTICES);
            mVertexBuffer.position(0);

            // Initialize texture coordinate buffer - using default texture coordinates
            updateTextureCoordinates(0);

            // Create vertex shader
            String vertexShaderCode = "attribute vec2 aPosition;\n"
                    + "attribute vec2 aTextureCoord;\n"
                    + "varying vec2 vTextureCoord;\n"
                    + "void main() {\n"
                    + "  gl_Position = vec4(aPosition, 0.0, 1.0);\n"
                    + "  vTextureCoord = aTextureCoord;\n"
                    + "}";

            // Create fragment shader
            String fragmentShaderCode = "precision mediump float;\n"
                    + "varying vec2 vTextureCoord;\n"
                    + "uniform sampler2D uTexture;\n"
                    + "void main() {\n"
                    + "  gl_FragColor = texture2D(uTexture, vTextureCoord);\n"
                    + "}";

            // Compile shaders
            int vertexShader = compileShader(GLES20.GL_VERTEX_SHADER, vertexShaderCode);
            int fragmentShader = compileShader(GLES20.GL_FRAGMENT_SHADER, fragmentShaderCode);

            // Create program
            mProgramHandle = GLES20.glCreateProgram();
            GLES20.glAttachShader(mProgramHandle, vertexShader);
            GLES20.glAttachShader(mProgramHandle, fragmentShader);
            GLES20.glLinkProgram(mProgramHandle);

            // Get attribute locations
            mPositionHandle = GLES20.glGetAttribLocation(mProgramHandle, "aPosition");
            mTextureCoordHandle = GLES20.glGetAttribLocation(mProgramHandle, "aTextureCoord");
            mTextureSamplerHandle = GLES20.glGetUniformLocation(mProgramHandle, "uTexture");

            // Create texture
            int[] textures = new int[1];
            GLES20.glGenTextures(1, textures, 0);
            mTextureID = textures[0];
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureID);

            // Set texture parameters
            GLES20.glTexParameteri(
                    GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MIN_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(
                    GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_MAG_FILTER, GLES20.GL_LINEAR);
            GLES20.glTexParameteri(
                    GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_S, GLES20.GL_CLAMP_TO_EDGE);
            GLES20.glTexParameteri(
                    GLES20.GL_TEXTURE_2D, GLES20.GL_TEXTURE_WRAP_T, GLES20.GL_CLAMP_TO_EDGE);
        }

        // Update texture coordinates based on rotation angle
        private void updateTextureCoordinates(int rotation) {
            boolean isFrontCamera = (mCamera2Helper != null)
                    && (Camera2Helper.CAMERA_FACING == CameraCharacteristics.LENS_FACING_FRONT);

            // Select appropriate texture coordinates
            switch (rotation) {
                case 90:
                    TEXTURE_COORDS = isFrontCamera ? TEXTURE_COORDS_270 : TEXTURE_COORDS_90;
                    break;
                case 180:
                    TEXTURE_COORDS = isFrontCamera ? TEXTURE_COORDS_180 : TEXTURE_COORDS_180;
                    break;
                case 270:
                    TEXTURE_COORDS = isFrontCamera ? TEXTURE_COORDS_90 : TEXTURE_COORDS_270;
                    break;
                default: // 0 degrees
                    TEXTURE_COORDS = isFrontCamera ? TEXTURE_COORDS_MIRROR_0 : TEXTURE_COORDS_0;
                    break;
            }

            // Update texture coordinate buffer
            ByteBuffer bb = ByteBuffer.allocateDirect(TEXTURE_COORDS.length * 4);
            bb.order(ByteOrder.nativeOrder());
            mTexCoordBuffer = bb.asFloatBuffer();
            mTexCoordBuffer.put(TEXTURE_COORDS);
            mTexCoordBuffer.position(0);
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            GLES20.glViewport(0, 0, width, height);

            // Store view dimensions for later calculations
            mViewWidth = width;
            mViewHeight = height;

            // If texture data already exists, update vertex coordinates to match texture aspect
            // ratio
            if (mTextureWidth > 0 && mTextureHeight > 0) {
                updateVertexCoordinates();
            }
        }

        // Update vertex coordinates to maintain video aspect ratio
        private void updateVertexCoordinates() {
            if (mViewWidth <= 0 || mViewHeight <= 0 || mTextureWidth <= 0 || mTextureHeight <= 0) {
                return;
            }

            // Calculate view and texture aspect ratios
            float viewAspectRatio = (float) mViewWidth / mViewHeight;
            float textureAspectRatio;

            // Consider rotation
            if (mRotation == 90 || mRotation == 270) {
                // Width and height are swapped after rotation
                textureAspectRatio = (float) mTextureHeight / mTextureWidth;
            } else {
                textureAspectRatio = (float) mTextureWidth / mTextureHeight;
            }

            // Set vertex coordinates while maintaining texture aspect ratio
            float[] adjustedVertices = new float[8];

            if (textureAspectRatio > viewAspectRatio) {
                // Texture is wider than view, adjust height
                float heightRatio = viewAspectRatio / textureAspectRatio;
                adjustedVertices[0] = -1.0f; // Bottom left x
                adjustedVertices[1] = -heightRatio; // Bottom left y
                adjustedVertices[2] = 1.0f; // Bottom right x
                adjustedVertices[3] = -heightRatio; // Bottom right y
                adjustedVertices[4] = -1.0f; // Top left x
                adjustedVertices[5] = heightRatio; // Top left y
                adjustedVertices[6] = 1.0f; // Top right x
                adjustedVertices[7] = heightRatio; // Top right y
            } else {
                // Texture is higher than view, adjust width
                float widthRatio = textureAspectRatio / viewAspectRatio;
                adjustedVertices[0] = -widthRatio; // Bottom left x
                adjustedVertices[1] = -1.0f; // Bottom left y
                adjustedVertices[2] = widthRatio; // Bottom right x
                adjustedVertices[3] = -1.0f; // Bottom right y
                adjustedVertices[4] = -widthRatio; // Top left x
                adjustedVertices[5] = 1.0f; // Top left y
                adjustedVertices[6] = widthRatio; // Top right x
                adjustedVertices[7] = 1.0f; // Top right y
            }

            // Update vertex buffer
            ByteBuffer bb = ByteBuffer.allocateDirect(adjustedVertices.length * 4);
            bb.order(ByteOrder.nativeOrder());
            mVertexBuffer = bb.asFloatBuffer();
            mVertexBuffer.put(adjustedVertices);
            mVertexBuffer.position(0);

            Log.d(TAG,
                    "Video aspect ratio updated: View=" + viewAspectRatio
                            + ", Texture=" + textureAspectRatio + ", Rotation=" + mRotation);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            // Clear screen
            GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT);

            synchronized (mLock) {
                if (mTextureNeedsUpdate && mTextureData != null) {
                    GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureID);
                    GLES20.glTexImage2D(GLES20.GL_TEXTURE_2D, 0, GLES20.GL_RGBA, mTextureWidth,
                            mTextureHeight, 0, GLES20.GL_RGBA, GLES20.GL_UNSIGNED_BYTE,
                            mTextureData);
                    mTextureNeedsUpdate = false;
                }
            }

            // Use shader program
            GLES20.glUseProgram(mProgramHandle);

            // Set vertex coordinates
            GLES20.glVertexAttribPointer(
                    mPositionHandle, 2, GLES20.GL_FLOAT, false, 0, mVertexBuffer);
            GLES20.glEnableVertexAttribArray(mPositionHandle);

            // Set texture coordinates
            GLES20.glVertexAttribPointer(
                    mTextureCoordHandle, 2, GLES20.GL_FLOAT, false, 0, mTexCoordBuffer);
            GLES20.glEnableVertexAttribArray(mTextureCoordHandle);

            // Set texture
            GLES20.glActiveTexture(GLES20.GL_TEXTURE0);
            GLES20.glBindTexture(GLES20.GL_TEXTURE_2D, mTextureID);
            GLES20.glUniform1i(mTextureSamplerHandle, 0);

            // Draw
            GLES20.glDrawArrays(GLES20.GL_TRIANGLE_STRIP, 0, 4);

            // Disable vertex array
            GLES20.glDisableVertexAttribArray(mPositionHandle);
            GLES20.glDisableVertexAttribArray(mTextureCoordHandle);
        }

        // Update texture data and handle rotation
        public void updateTextureData(byte[] data, int width, int height, int sensorOrientation) {
            synchronized (mLock) {
                boolean sizeChanged = (mTextureWidth != width || mTextureHeight != height);

                if (mTextureData == null || sizeChanged) {
                    mTextureData = ByteBuffer.allocateDirect(width * height * 4);
                    mTextureData.order(ByteOrder.nativeOrder());
                    mTextureWidth = width;
                    mTextureHeight = height;
                }

                mTextureData.clear();
                mTextureData.put(data);
                mTextureData.position(0);
                mTextureNeedsUpdate = true;

                // When rotation changes, update texture coordinates
                int rotation = getRotationDegrees(sensorOrientation);
                boolean rotationChanged = (rotation != mRotation);

                if (rotationChanged) {
                    mRotation = rotation;
                    updateTextureCoordinates(mRotation);
                }

                // If size or rotation changes, update vertex coordinates to maintain aspect ratio
                if (sizeChanged || rotationChanged) {
                    updateVertexCoordinates();
                }
            }
        }

        // Get appropriate rotation angle based on sensor orientation
        private int getRotationDegrees(int sensorOrientation) {
            // Determine rotation angle, considering device orientation and front/rear camera
            int deviceOrientation = getWindowManager().getDefaultDisplay().getRotation() * 90;

            // Front camera needs special handling
            boolean isFrontCamera = (mCamera2Helper != null)
                    && (Camera2Helper.CAMERA_FACING == CameraCharacteristics.LENS_FACING_FRONT);

            int rotationDegrees;
            if (isFrontCamera) {
                rotationDegrees = (sensorOrientation + deviceOrientation) % 360;
            } else {
                rotationDegrees = (sensorOrientation - deviceOrientation + 360) % 360;
            }

            return rotationDegrees;
        }

        // Compile shader
        private int compileShader(int type, String shaderCode) {
            int shader = GLES20.glCreateShader(type);
            GLES20.glShaderSource(shader, shaderCode);
            GLES20.glCompileShader(shader);

            final int[] compileStatus = new int[1];
            GLES20.glGetShaderiv(shader, GLES20.GL_COMPILE_STATUS, compileStatus, 0);

            if (compileStatus[0] == 0) {
                Log.e(TAG, "Error compiling shader: " + GLES20.glGetShaderInfoLog(shader));
                GLES20.glDeleteShader(shader);
                return 0;
            }

            return shader;
        }
    }

    public void checkCameraPermission() {
        // Check camera permission
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA)
                != PackageManager.PERMISSION_GRANTED) {
            // If no camera permission, request permission
            ActivityCompat.requestPermissions(this, new String[] {Manifest.permission.CAMERA},
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
        mGLSurfaceView.onResume();

        if (mCamera2Helper != null && !mCamera2Helper.isCameraOpened()) {
            mCamera2Helper.startCamera();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        mGLSurfaceView.onPause();

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

        if (mSinkRawData != null) {
            mSinkRawData.Destroy();
            mSinkRawData = null;
        }

        super.onDestroy();
    }
}
