/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.util.Log;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class GPUPixel {
    private static final String TAG = "GPUPixel";

    static {
        System.loadLibrary("gpupixel");
        System.loadLibrary("mars-face-kit");
    }

    private GPUPixel() {}

    public static void Init(Context context) {
        copyResource(context);
    }

    public void Destroy() {}

    private static String resource_path = "";
    public static void copyResource(Context context) {
        if (context == null) {
            Log.e(TAG, "Context is null in copyResource");
            return;
        }

        try {
            String exPath = context.getExternalFilesDir(null).getAbsolutePath();
            copyAssetsToFiles(context, "resource", exPath + "/resource");
            resource_path = exPath + "/resource";
            Log.i(TAG, "Resource path set to: " + resource_path);
        } catch (Exception e) {
            Log.e(TAG, "Error in copyResource: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public static String getResource_path() {
        if (resource_path.isEmpty()) {
            Log.w(TAG, "Resource path is empty, returning default path");
            // Return a default path to avoid crashes from empty string
            return "/sdcard/Android/data/com.pixpark.gpupixel/files/resource";
        }
        Log.i(TAG, "Returning resource path: " + resource_path);
        return resource_path;
    }

    public static void copyAssetsToFiles(Context context, String oldPath, String newPath) {
        try {
            String fileNames[] = context.getAssets().list(
                    oldPath); // Get all files and directory names under assets directory
            if (fileNames.length > 0) { // If it's a directory
                File file = new File(newPath);
                if (!file.exists()) {
                    file.mkdirs(); // Create directories recursively if the folder doesn't exist
                }
                for (String fileName : fileNames) {
                    String srcPath = oldPath + "/" + fileName;
                    String dstPath = newPath + "/" + fileName;
                    File f = new File(dstPath);
                    if (f.exists()) continue;
                    copyAssetsToFiles(context, srcPath, dstPath);
                }

            } else { // If it's a file
                File file = new File(newPath);
                if (!file.exists()) {
                    InputStream is = context.getAssets().open(oldPath);
                    FileOutputStream fos = new FileOutputStream(file);
                    byte[] buffer = new byte[1024];
                    int byteCount = 0;
                    while ((byteCount = is.read(buffer))
                            != -1) { // Loop to read bytes from input stream
                        fos.write(buffer, 0,
                                byteCount); // Write the read input stream to the output stream
                    }
                    fos.flush(); // Flush buffer
                    is.close();
                    fos.close();
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in copyAssetsToFiles: " + e.getMessage());
            e.printStackTrace();
        }
    }
}
