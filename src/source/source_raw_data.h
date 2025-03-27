/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gpupixel_program.h"
#include <functional>
namespace gpupixel {
class GPUPIXEL_API SourceRawData : public Filter {
 public:
  ~SourceRawData();
  static std::shared_ptr<SourceRawData> create();
  void processData(const uint8_t* pixels,
                   int width,
                   int height,
                   int stride);
  void processData(int width,
                   int height,
                   const uint8_t* dataY,
                   int strideY,
                   const uint8_t* dataU,
                   int strideU,
                   const uint8_t* dataV,
                   int strideV);

  void setRotation(RotationMode rotation);

 private:
  SourceRawData();
  bool init();

  int genTextureWithI420(int width,
                         int height,
                         const uint8_t* dataY,
                         int strideY,
                         const uint8_t* dataU,
                         int strideU,
                         const uint8_t* dataV,
                         int strideV);

  int genTextureWithRGBA(const uint8_t* pixels,
                         int width,
                         int height,
                         int stride);

 private:
  GPUPixelGLProgram* _filterProgram;
  GLuint _filterPositionAttribute;
  GLuint _filterTexCoordAttribute;

  GLuint _textures[4] = {0};
  RotationMode _rotation = NoRotation;
  std::shared_ptr<GPUPixelFramebuffer> _framebuffer;
};

}
