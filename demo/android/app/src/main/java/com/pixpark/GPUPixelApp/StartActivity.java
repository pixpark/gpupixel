package com.pixpark.GPUPixelApp;

import android.content.Intent;
import android.os.Bundle;

import androidx.annotation.Nullable;
import androidx.appcompat.app.AppCompatActivity;

import com.pixpark.GPUPixelApp.databinding.ActivityStartBinding;


/**
 * Created by linchuangui on 2025/9/15.
 */
public class StartActivity extends AppCompatActivity {

    private ActivityStartBinding binding;

    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        binding = ActivityStartBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        binding.btnStart.setOnClickListener(v -> {
            startActivity(new Intent(this, MainActivity.class));
        });
    }
}
