/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include <functional>

#include "gpupixel/filter/filter.h"
#include "gpupixel/source/source.h"

namespace gpupixel {
class GPUPixelGLProgram;
class GPUPIXEL_API SourceRawData : public Filter {
 public:
  static std::shared_ptr<SourceRawData> Create();

  ~SourceRawData() override;

  void ProcessData(const uint8_t* data,
                   int width,
                   int height,
                   int stride,
                   GPUPIXEL_FRAME_TYPE type);

  void SetRotation(RotationMode rotation);

  bool Init();

 private:
  SourceRawData();
  int GenerateTextureWithI420(int width,
                              int height,
                              const uint8_t* dataY,
                              int strideY,
                              const uint8_t* dataU,
                              int strideU,
                              const uint8_t* dataV,
                              int strideV);

  int GenerateTextureWithPixels(const uint8_t* pixels,
                                int width,
                                int height,
                                int stride,
                                GPUPIXEL_FRAME_TYPE type);

 private:
  GPUPixelGLProgram* filter_program_;
  uint32_t filter_position_attribute_;
  uint32_t filter_tex_coord_attribute_;

  uint32_t textures_[4] = {0};
  RotationMode rotation_ = NoRotation;
  std::shared_ptr<GPUPixelFramebuffer> framebuffer_;
};

}  // namespace gpupixel
