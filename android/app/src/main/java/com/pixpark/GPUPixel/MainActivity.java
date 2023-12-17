package com.pixpark.PixDemo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.View;
import android.view.WindowManager;
import android.widget.Button;

import com.pixpark.PixDemo.databinding.ActivityMainBinding;
import com.pixpark.gpupixel.GPUPixelFilter;
import com.pixpark.gpupixel.GPUPixelSourceCamera;
import com.pixpark.gpupixel.GPUPixelView;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    // Used to load the 'gpupixel' library on application startup.
    static {
        System.loadLibrary("gpupixel");
    }

    private GPUPixelSourceCamera sourceCamera;
    private GPUPixelView surfaceView;
    private GPUPixelFilter filter;
    private Button btnStart;

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        // 保持屏幕常亮
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // preview
        surfaceView = findViewById(R.id.surfaceView);

        // 美颜滤镜
        filter = GPUPixelFilter.create("FaceBeautyFilter");

        // camera
        sourceCamera = new GPUPixelSourceCamera(this.getApplicationContext());
        sourceCamera.addTarget(filter);
        filter.addTarget(surfaceView);

        //
        btnStart = findViewById(R.id.btnStart);
        btnStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

            }
        });
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