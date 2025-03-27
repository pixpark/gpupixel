//
//  source_raw_output.h
//  GPUPixel iOS
//
//  Created by PixPark on 2021/6/24.
//  Copyright © 2021 PixPark. All rights reserved.
//

#pragma once

#include <stdio.h>
#include "gpupixel_program.h"
#include "sink.h"
#include <functional>

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#endif

#include <mutex>
namespace gpupixel {
GPUPIXEL_API typedef std::function<
    void(const uint8_t* data, int width, int height, int64_t ts)>
    RawOutputCallback;
    
#define PBO_SIZE 2

class GPUPIXEL_API SinkRawData : public Sink {
 public:
  SinkRawData();
  virtual ~SinkRawData();
  static std::shared_ptr<SinkRawData> create();
  void render() override;
  void setI420Callbck(RawOutputCallback cb);
  void setPixelsCallbck(RawOutputCallback cb);
 private:
  int renderToOutput();
  bool initWithShaderString(const std::string& vertexShaderSource,
                            const std::string& fragmentShaderSource);
  void initTextureCache(int width, int height);
  void initFrameBuffer(int width, int height);
#if defined(GPUPIXEL_IOS)
  void readPixelsFromCVPixelBuffer();
#endif
  void initOutputBuffer(int width, int height);
  void initPBO(int width, int height);
  void readPixelsWithPBO(int width, int height);

 private:
  std::mutex mtx_;
  GPUPixelGLProgram* _filterProgram;
  GLuint _filterPositionAttribute;
  GLuint _filterTexCoordAttribute;
  //
#if defined(GPUPIXEL_IOS)
  GLuint _framebuffer = 0;
  CVOpenGLESTextureRef renderTexture = NULL;
  CVOpenGLESTextureCacheRef textureCache = NULL;
  CVPixelBufferRef renderTarget = NULL;
#else
  std::shared_ptr<GPUPixelFramebuffer> _framebuffer;
#endif

  bool init_ = false;

  GLuint pboIds[PBO_SIZE] = {0};

  GLuint pboIds_yuvdata[PBO_SIZE] = {0};

  int32_t index = 0;
  int32_t nextIndex = 0;

  // iamge width & height
  int32_t _width = 0;
  int32_t _height = 0;
  int64_t _frame_ts = 0;

  // rgb buffer
  uint8_t* _readPixelData = nullptr;
  uint8_t* _yuvFrameBuffer = nullptr;
  // callback
  RawOutputCallback i420_callback_ = nullptr;
  RawOutputCallback pixels_callback_ = nullptr;

  bool current_frame_invalid_ = true;
};

}
