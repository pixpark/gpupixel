/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <stdlib.h>
#include <functional>
#include <vector>
#include "gpupixel_define.h"
namespace mars_face_kit {
    class MarsFaceDetector;
}

namespace gpupixel {

class GPUPIXEL_API FaceDetector {
    public:
        FaceDetector();

        ~FaceDetector();

        std::vector<float> Detect(const uint8_t* data,
                    int width,
                    int height,
                    GPUPIXEL_MODE_FMT fmt,
                    GPUPIXEL_FRAME_TYPE type);
      
    private:
        std::shared_ptr<mars_face_kit::MarsFaceDetector> mars_face_detector_;
    };
}
