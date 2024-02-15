/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <stdlib.h>
#include <functional>
#include "gpupixel_macros.h"

NS_GPUPIXEL_BEGIN
    typedef std::function<void(std::vector<float> landmarks)> FaceDetectorCallback;

    typedef enum {
        GPUPIXEL_FRAME_TYPE_UNKNOW,      /*  Unknow pixel format, as a cube */
        GPUPIXEL_FRAME_TYPE_YUVI420,     /*  YUV  4:2:0   12bpp ( 3 planes, the first is Y, the second is U, the third is V */
        GPUPIXEL_FRAME_TYPE_RGBA8888,    /*  RGBA 8:8:8:8 32bpp ( 4 channel, 8x4=32bit RGBA pixel ) */  
        GPUPIXEL_FRAME_TYPE_RGB888  
    } GPUPIXEL_FRAME_TYPE;

    class FaceDetector {
    public:
        FaceDetector();

        ~FaceDetector();

        int Detect(const uint8_t* data,
                    int width,
                    int height,
                    GPUPIXEL_FRAME_TYPE type);
      
        int RegCallback(FaceDetectorCallback callback);
    private:
        uint32_t vnn_handle_;
        int use_278pts = 0;
        std::vector<FaceDetectorCallback> _face_detector_callbacks;
        
    };
NS_GPUPIXEL_END
