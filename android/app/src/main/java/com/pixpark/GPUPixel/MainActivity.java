package com.pixpark.PixDemo;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.SeekBar;

import com.pixpark.PixDemo.databinding.ActivityMainBinding;
import com.pixpark.gpupixel.GPUPixel;
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
    private SeekBar smooth_seekbar;
    private SeekBar whiteness_seekbar;

    private ActivityMainBinding binding;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());

        String path =  getExternalFilesDir("gpupixel").getAbsolutePath();
        Log.i("GPUPixel", path);

        // 保持屏幕常亮
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);

        // must call fist
        GPUPixel.setAppContext(this);
        // preview
        surfaceView = findViewById(R.id.surfaceView);

        // 美颜滤镜
        filter = GPUPixelFilter.create("FaceBeautyFilter");

        // camera
        sourceCamera = new GPUPixelSourceCamera(this.getApplicationContext());
        sourceCamera.addTarget(filter);
        filter.addTarget(surfaceView);

        // set default value
        filter.setProperty("skin_smoothing", 0.5);
        filter.setProperty("whiteness", 0.4);

        smooth_seekbar = findViewById(R.id.smooth_seekbar);
        smooth_seekbar.setOnSeekBarChangeListener(new SeekBar.OnSeekBarChangeListener() {
            @Override
            public void onProgressChanged(SeekBar seekBar, int progress, boolean fromUser) {
                filter.setProperty("skin_smoothing", progress/10.0);
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
                filter.setProperty("whiteness", progress/10.0);
            }

            @Override
            public void onStartTrackingTouch(SeekBar seekBar) {

            }

            @Override
            public void onStopTrackingTouch(SeekBar seekBar) {

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