/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;


import android.opengl.GLSurfaceView.Renderer;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;
import java.util.Queue;
import java.util.LinkedList;

public class GPUPixelRenderer implements Renderer {
    private final Queue<Runnable> mPreDrawQueue;
    private final Queue<Runnable> mDrawQueue;
    private final Queue<Runnable> mPostDrawQueue;

    private GPUPixelSource mSource = null;

    public GPUPixelRenderer() {
        mPreDrawQueue = new LinkedList<Runnable>();
        mDrawQueue = new LinkedList<Runnable>();
        mPostDrawQueue = new LinkedList<Runnable>();
    }

    public void setSource(GPUPixelSource source) {
        mSource = source;
    }

    @Override
    public void onSurfaceCreated(GL10 gl, EGLConfig config) {
    }

    @Override
    public void onSurfaceChanged(GL10 gl, int width, int height) {
        if (mSource != null) {
            mSource.proceed(true, false);
        }
    }

    @Override
    public void onDrawFrame(GL10 gl) {
        runAll(mPreDrawQueue);
        runAll(mDrawQueue);
        runAll(mPostDrawQueue);
    }

    private void runAll(Queue<Runnable> queue) {
        synchronized (queue) {
            while (!queue.isEmpty()) {
                queue.poll().run();
            }
        }
    }

    protected boolean isPreDrawQueueEmpty() {
        synchronized (mPreDrawQueue) {
            return mPreDrawQueue.isEmpty();
        }
    }

    protected void runOnPreDraw(final Runnable runnable) {
        synchronized (mPreDrawQueue) {
            mPreDrawQueue.add(runnable);
        }
    }

    protected boolean isDrawQueueEmpty() {
        synchronized (mDrawQueue) {
            return mDrawQueue.isEmpty();
        }
    }

    protected void runOnDraw(final Runnable runnable) {
        synchronized (mDrawQueue) {
            mDrawQueue.add(runnable);
        }
    }

    protected boolean isPostDrawQueueEmpty() {
        synchronized (mPostDrawQueue) {
            return mPostDrawQueue.isEmpty();
        }
    }

    protected void runOnPostDraw(final Runnable runnable) {
        synchronized (mPostDrawQueue) {
            mPostDrawQueue.add(runnable);
        }
    }

    public void clear() {
        mPreDrawQueue.clear();
        mDrawQueue.clear();
        mPostDrawQueue.clear();
    }
}
