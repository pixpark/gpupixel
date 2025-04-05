/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

package com.pixpark.gpupixel;

/**
 * GPUPixelSink interface, base interface for all output sink classes
 */
public interface GPUPixelSink {
    /**
     * Get native class ID
     */
    long getNativeClassID();
}
