/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "filter.h"
#include "gl_program.h"
#include <functional>
NS_GPUPIXEL_BEGIN

class SourceRawDataInput : public Filter {
 public:
  ~SourceRawDataInput();
  static std::shared_ptr<SourceRawDataInput> create();
  void uploadBytes(const uint8_t* pixels,
                   int width,
                   int height,
                   int stride,
                   int64_t ts = 0);
  void uploadBytes(int width,
                   int height,
                   const uint8_t* dataY,
                   int strideY,
                   const uint8_t* dataU,
                   int strideU,
                   const uint8_t* dataV,
                   int strideV,
                   int64_t ts = 0);

  void setRotation(RotationMode rotation);

 private:
  SourceRawDataInput();
  bool init();

  int genTextureWithI420(int width,
                         int height,
                         const uint8_t* dataY,
                         int strideY,
                         const uint8_t* dataU,
                         int strideU,
                         const uint8_t* dataV,
                         int strideV,
                         int64_t ts = 0);

  int genTextureWithRGBA(const uint8_t* pixels,
                         int width,
                         int height,
                         int stride,
                         int64_t ts = 0);

 private:
  GLProgram* _filterProgram;
  GLuint _filterPositionAttribute;
  GLuint _filterTexCoordAttribute;

  GLuint _textures[4] = {0};
  RotationMode _rotation = NoRotation;
  std::shared_ptr<Framebuffer> _framebuffer;
};

NS_GPUPIXEL_END
