package com.pixpark.PixDemo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.Button;

import com.pixpark.PixDemo.databinding.ActivityMainBinding;
import com.pixpark.gpupixel.GPUPixelFilter;
import com.pixpark.gpupixel.GPUPixelSourceCamera;
import com.pixpark.gpupixel.GPUPixelSourceRawInput;
import com.pixpark.gpupixel.GPUPixelView;

public class MainActivity extends AppCompatActivity implements SurfaceHolder.Callback {

    // Used to load the 'gpupixel' library on application startup.
    static {
        System.loadLibrary("gpupixel");
    }

    private GPUPixelSourceCamera gpuImageSourceCamera;
    private GPUPixelSourceRawInput gpuImageSourceRawInput;
    private GPUPixelView surfaceView;
    private GPUPixelFilter filter;
    private Button btnStart;
    private boolean isBeautyFaceOpen;

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        isBeautyFaceOpen = true;

        gpuImageSourceCamera = new GPUPixelSourceCamera(this.getApplicationContext());
        gpuImageSourceRawInput = new GPUPixelSourceRawInput();
        gpuImageSourceCamera.SetSourceRawInput(gpuImageSourceRawInput);
        surfaceView = findViewById(R.id.surfaceView);
        btnStart = findViewById(R.id.btnStart);
        String path =  getExternalFilesDir("gpupixel").getAbsolutePath();
        Log.i("GPUPixel", path);
        btnStart.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if(isBeautyFaceOpen) {
                    isBeautyFaceOpen = false;
                    filter = GPUPixelFilter.create("BeautifyFilter");
                    //gpuImageSourceCamera.addTarget(filter);
                    gpuImageSourceRawInput.addTarget(filter);
                    filter.addTarget(surfaceView);
                } else {
                    isBeautyFaceOpen = true;
//                    gpuImageSourceCamera.addTarget(surfaceView);
                }

            }
        });

//        surfaceView.getHolder().addCallback(this);
    }



    public void surfaceCreated(SurfaceHolder holder) {
        gpuImageSourceCamera.setPreviewHolder(holder);
    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder holder, int format, int width, int height) {

    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder holder) {

    }
}