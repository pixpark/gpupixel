/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#ifdef __emscripten__
#include <emscripten.h>
#include <emscripten/bind.h>
#endif

#include <functional>
#include "filter.h"
#include "gl_program.h"
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

#ifdef __emscripten__
class SourceRawDataInputWrapper {
 public:
  SourceRawDataInputWrapper() { input = SourceRawDataInput::create(); }

  void uploadRGBBytes(const emscripten::val& pixelsArray,
                      int width,
                      int height,
                      int stride) {
    size_t length = pixelsArray["length"].as<size_t>();
    uint8_t* pixels = new uint8_t[length];
    emscripten::val memoryView =
        emscripten::val(emscripten::typed_memory_view(length, pixels));
    memoryView.call<void>("set", pixelsArray);

    input->uploadBytes(pixels, width, height, stride, 0);
    delete[] pixels;
  }
  void uploadYUVBytes(int width,
                      int height,
                      const emscripten::val& yArray,
                      int strideY,
                      const emscripten::val& uArray,
                      int strideU,
                      const emscripten::val& vArray,
                      int strideV,
                      int64_t ts = 0) {
    size_t yLength = yArray["length"].as<size_t>();
    uint8_t* dataY = new uint8_t[yLength];
    emscripten::val memoryYView =
        emscripten::val(emscripten::typed_memory_view(yLength, dataY));
    memoryYView.call<void>("set", yArray);

    size_t uLength = uArray["length"].as<size_t>();
    uint8_t* dataU = new uint8_t[uLength];
    emscripten::val memoryUView =
        emscripten::val(emscripten::typed_memory_view(uLength, dataU));
    memoryUView.call<void>("set", uArray);

    size_t vLength = vArray["length"].as<size_t>();
    uint8_t* dataV = new uint8_t[vLength];
    emscripten::val memoryVView =
        emscripten::val(emscripten::typed_memory_view(vLength, dataV));
    memoryVView.call<void>("set", vArray);

    input->uploadBytes(width, height, dataY, strideY, dataU, strideU, dataV,
                       strideV, ts);

    delete[] dataY;
    delete[] dataU;
    delete[] dataV;
  }

  void addTarget(std::shared_ptr<Target> target) { input->addTarget(target); }
  void setRotation(RotationMode rotation) { input->setRotation(rotation); }

  std::shared_ptr<SourceRawDataInput> input;
};
#endif

NS_GPUPIXEL_END
