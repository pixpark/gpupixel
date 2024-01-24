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
import android.widget.Button;
import android.widget.SeekBar;
import android.widget.Toast;

import com.pixpark.GPUPixelApp.databinding.ActivityMainBinding;
import com.pixpark.gpupixel.GPUPixelFilter;
import com.pixpark.gpupixel.GPUPixelSourceCamera;
import com.pixpark.gpupixel.GPUPixelView;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {
    private static final int CAMERA_PERMISSION_REQUEST_CODE = 200;
    private static final String TAG = "GPUPixelDemo";

    private GPUPixelSourceCamera sourceCamera;
    private GPUPixelView surfaceView;
    private GPUPixelFilter beautyFaceFilter;
    private Button btnStart;
    private SeekBar smooth_seekbar;
    private SeekBar whiteness_seekbar;

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // get log path
        String path =  getExternalFilesDir("gpupixel").getAbsolutePath();
        Log.i(TAG, path);

        // 保持屏幕常亮
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // preview
        surfaceView = findViewById(R.id.surfaceView);
        surfaceView.setMirror(true);

        smooth_seekbar = findViewById(R.id.smooth_seekbar);
        smooth_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                beautyFaceFilter.setProperty("skin_smoothing", progress/10.0);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

            }
        });

        whiteness_seekbar = findViewById(R.id.whiteness_seekbar);
        whiteness_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                beautyFaceFilter.setProperty("whiteness", progress/10.0);
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
        beautyFaceFilter = GPUPixelFilter.create(GPUPixelFilter.BeautyFaceFilter.name);

        // camera
        sourceCamera = new GPUPixelSourceCamera(this.getApplicationContext());
        sourceCamera.addTarget(beautyFaceFilter);
        beautyFaceFilter.addTarget(surfaceView);

        // set default value
        beautyFaceFilter.setProperty(GPUPixelFilter.BeautyFaceFilter.propSmoothLevel, 0.5);
        beautyFaceFilter.setProperty(GPUPixelFilter.BeautyFaceFilter.propWhiteLevel, 0.4);
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
}