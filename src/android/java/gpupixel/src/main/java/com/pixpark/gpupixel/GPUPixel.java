/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

import android.app.Activity;
import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.PixelFormat;
import android.hardware.camera2.CameraCharacteristics;
import android.media.Image;
import android.opengl.GLSurfaceView;
import android.os.Build;
import android.util.Log;
import android.view.Surface;
import android.view.WindowManager;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.nio.ByteBuffer;

/**
 * Main entry point for GPU image processing operations
 */
public class GPUPixel {
    private static final String TAG = "GPUPixel";

    static {
        System.loadLibrary("gpupixel");
        System.loadLibrary("mars-face-kit");
    }

    private GPUPixel() {}

    /**
     * Initializes the GPUPixel library
     * @param context Application context
     */
    public static void Init(Context context) {
        copyResource(context);
    }

    public void Destroy() {}

    private static String resource_path = "";

    /**
     * Copies required resources from assets to external storage
     * @param context Application context
     */
    public static void copyResource(Context context) {
        if (context == null) {
            Log.e(TAG, "Context is null in copyResource");
            return;
        }

        try {
            String exPath = context.getExternalFilesDir(null).getAbsolutePath();
            copyAssetsToFiles(context, "resource", exPath + "/resource");
            resource_path = exPath + "/resource";
        } catch (Exception e) {
            Log.e(TAG, "Error in copyResource: " + e.getMessage());
        }
    }

    /**
     * Gets the resource path
     * @return Path to resources
     */
    public static String getResource_path() {
        if (resource_path.isEmpty()) {
            // Return a default path to avoid crashes from empty string
            return "/sdcard/Android/data/com.pixpark.gpupixel/files/resource";
        }
        return resource_path;
    }

    /**
     * Copies files from assets to external storage
     * @param context Application context
     * @param oldPath Source path in assets
     * @param newPath Destination path in external storage
     */
    public static void copyAssetsToFiles(Context context, String oldPath, String newPath) {
        try {
            String fileNames[] = context.getAssets().list(oldPath);
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
                    while ((byteCount = is.read(buffer)) != -1) {
                        fos.write(buffer, 0, byteCount);
                    }
                    fos.flush();
                    is.close();
                    fos.close();
                }
            }
        } catch (Exception e) {
            Log.e(TAG, "Error in copyAssetsToFiles: " + e.getMessage());
        }
    }

    /**
     * Converts YUV_420_888 format image to RGBA format
     *
     * @param image YUV_420_888 format image
     * @return RGBA format byte array
     */
    public static byte[] YUV_420_888toRGBA(Image image) {
        if (image == null) return null;

        int width = image.getWidth();
        int height = image.getHeight();

        Image.Plane[] planes = image.getPlanes();
        ByteBuffer yBuffer = planes[0].getBuffer();
        ByteBuffer uBuffer = planes[1].getBuffer();
        ByteBuffer vBuffer = planes[2].getBuffer();

        int yRowStride = planes[0].getRowStride();
        int uRowStride = planes[1].getRowStride();
        int vRowStride = planes[2].getRowStride();

        int yPixelStride = planes[0].getPixelStride();
        int uPixelStride = planes[1].getPixelStride();
        int vPixelStride = planes[2].getPixelStride();

        byte[] rgba = new byte[width * height * 4];

        nativeYUV420ToRGBA(yBuffer, uBuffer, vBuffer, width, height, yRowStride, uRowStride,
                vRowStride, yPixelStride, uPixelStride, vPixelStride, rgba);

        return rgba;
    }

    /**
     * Rotates RGBA format image
     *
     * @param rgba Original RGBA format image data
     * @param width Image width
     * @param height Image height
     * @param rotation Rotation angle (0, 90, 180, 270)
     * @return Rotated RGBA image data
     */
    public static byte[] rotateRgbaImage(byte[] rgba, int width, int height, int rotation) {
        if (rgba == null || rgba.length != width * height * 4) {
            Log.e(TAG, "Invalid RGBA data or dimensions mismatch");
            return rgba;
        }

        if (rotation == 0 || rotation == 360) {
            return rgba;
        }

        int outWidth = (rotation == 90 || rotation == 270) ? height : width;
        int outHeight = (rotation == 90 || rotation == 270) ? width : height;

        byte[] rotatedData = new byte[outWidth * outHeight * 4];

        nativeRotateRGBA(rgba, width, height, rotatedData, outWidth, outHeight, rotation);

        return rotatedData;
    }

    /**
     * Calculates image rotation angle
     *
     * @param activity Current Activity
     * @param sensorOrientation Camera sensor orientation
     * @param isFrontCamera Whether it's a front camera
     * @return Angle by which the image needs to be rotated
     */
    public static int calculateRotation(
            Activity activity, int sensorOrientation, boolean isFrontCamera) {
        int displayRotation = activity.getWindowManager().getDefaultDisplay().getRotation();
        int deviceRotation = 0;
        switch (displayRotation) {
            case Surface.ROTATION_0:
                deviceRotation = 0;
                break;
            case Surface.ROTATION_90:
                deviceRotation = 90;
                break;
            case Surface.ROTATION_180:
                deviceRotation = 180;
                break;
            case Surface.ROTATION_270:
                deviceRotation = 270;
                break;
        }

        int rotation;
        if (isFrontCamera) {
            rotation = (sensorOrientation + deviceRotation) % 360;
        } else {
            rotation = (sensorOrientation - deviceRotation + 360) % 360;
        }

        return rotation;
    }

    /**
     * Checks if the camera is front-facing
     *
     * @param facing Camera facing direction
     * @return Whether it's a front-facing camera
     */
    public static boolean isFrontCamera(int facing) {
        return facing == CameraCharacteristics.LENS_FACING_FRONT;
    }

    // JNI Native methods
    private static native void nativeYUV420ToRGBA(ByteBuffer yBuffer, ByteBuffer uBuffer,
            ByteBuffer vBuffer, int width, int height, int yRowStride, int uRowStride,
            int vRowStride, int yPixelStride, int uPixelStride, int vPixelStride, byte[] rgbaOut);

    private static native void nativeRotateRGBA(byte[] rgbaIn, int width, int height,
            byte[] rgbaOut, int outWidth, int outHeight, int rotationDegrees);
}
