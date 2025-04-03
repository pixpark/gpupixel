package com.pixpark.GPUPixelApp;

import static android.widget.Toast.LENGTH_LONG;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import android.widget.SeekBar;
import android.widget.Toast;
import java.nio.ByteBuffer;

import com.pixpark.GPUPixelApp.databinding.ActivityMainBinding;
import com.pixpark.gpupixel.GPUPixel;
import com.pixpark.gpupixel.filter.BeautyFaceFilter;
import com.pixpark.gpupixel.filter.FaceReshapeFilter;
import com.pixpark.gpupixel.GPUPixelSourceCamera;
import com.pixpark.gpupixel.GPUPixelView;
import com.pixpark.gpupixel.filter.LipstickFilter;
import com.pixpark.gpupixel.FaceDetector;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    private static final int CAMERA_PERMISSION_REQUEST_CODE = 200;
    private static final String TAG = "GPUPixelDemo";

    private GPUPixelSourceCamera sourceCamera;
    private GPUPixelView surfaceView;
    private BeautyFaceFilter beautyFaceFilter;
    private FaceReshapeFilter faceReshapFilter;
    private LipstickFilter lipstickFilter;
    private SeekBar smooth_seekbar;
    private SeekBar whiteness_seekbar;
    private SeekBar face_reshap_seekbar;
    private SeekBar big_eye_seekbar;
    private SeekBar lipstick_seekbar;
    
    // 添加人脸检测器
    private FaceDetector faceDetector;

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // get log path
        String path = getExternalFilesDir("gpupixel").getAbsolutePath();
        Log.i(TAG, path);

        GPUPixel.setContext(this);
        // 保持屏幕常亮
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // preview
        surfaceView = binding.surfaceView;
        surfaceView.SetMirror(true);

        smooth_seekbar = binding.smoothSeekbar;
        smooth_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                beautyFaceFilter.setSmoothLevel(progress / 10.0f);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        whiteness_seekbar = binding.whitenessSeekbar;
        whiteness_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                beautyFaceFilter.setWhiteLevel(progress / 10.0f);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });


        face_reshap_seekbar = binding.thinfaceSeekbar;
        face_reshap_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                faceReshapFilter.setThinLevel(progress / 200.0f);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        big_eye_seekbar = binding.bigeyeSeekbar;
        big_eye_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                faceReshapFilter.setBigeyeLevel(progress / 50.0f);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        lipstick_seekbar = binding.lipstickSeekbar;
        lipstick_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                lipstickFilter.SetBlendLevel(progress / 10.0f);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        //
        this.checkCameraPermission();

    }

    public void startCameraFilter() {
        // 美颜滤镜
        beautyFaceFilter = new BeautyFaceFilter();
        faceReshapFilter = new FaceReshapeFilter();
        lipstickFilter = new LipstickFilter();
        
        // 初始化人脸检测器
        faceDetector = new FaceDetector();
        
        // camera
        sourceCamera = new GPUPixelSourceCamera(this.getApplicationContext());
        
        // 设置数据回调，进行人脸检测
        sourceCamera.setFrameDataCallback(new GPUPixelSourceCamera.FrameDataCallback() {
            @Override
            public void onFrameData(ByteBuffer rgbaData, int width, int height) {
                // 获取ByteBuffer的数据
                byte[] byteArray = new byte[rgbaData.remaining()];
                rgbaData.get(byteArray);
                
                // 进行人脸检测
                float[] landmarks = faceDetector.detect(
                    byteArray, 
                    width, 
                    height, 
                    FaceDetector.GPUPIXEL_MODE_FMT_VIDEO,
                    FaceDetector.GPUPIXEL_FRAME_TYPE_RGBA
                );
                
                if (landmarks != null && landmarks.length > 0) {
                    Log.d(TAG, "检测到人脸特征点: " + landmarks.length);
                    // 将landmarks应用到滤镜中
                    lipstickFilter.setFaceLandmark(landmarks);
                    faceReshapFilter.setFaceLandmark(landmarks);
                }
            }
        });

        //
        sourceCamera.AddSink(lipstickFilter);
        lipstickFilter.AddSink(faceReshapFilter);
        faceReshapFilter.AddSink(beautyFaceFilter);
        beautyFaceFilter.AddSink(surfaceView);

        // set default value
        beautyFaceFilter.setSmoothLevel(0.5f);
        beautyFaceFilter.setWhiteLevel(0.4f);
    }

    public void checkCameraPermission() {
        // 检查相机权限是否已授予
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            // 如果未授予相机权限，向用户请求权限
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.CAMERA}, CAMERA_PERMISSION_REQUEST_CODE);
        } else {
            startCameraFilter();
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
        if (requestCode == CAMERA_PERMISSION_REQUEST_CODE) {
            if (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                startCameraFilter();
            } else {
                Toast.makeText(this, "No Camera permission!", LENGTH_LONG);
            }
        }
    }


    public void surfaceCreated(SurfaceHolder holder) {
        sourceCamera.setPreviewHolder(holder);
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }

    @Override
    protected void onDestroy() {
        // 销毁FaceDetector
        if (faceDetector != null) {
            faceDetector.destroy();
            faceDetector = null;
        }
        super.onDestroy();
    }
}