//
//  SourceRawOutput.h
//  GPUPixel iOS
//
//  Created by gezhaoyou on 2021/6/24.
//  Copyright © 2021 PixPark. All rights reserved.
//

#pragma once

#include <stdio.h>
#include "GLProgram.h"
#include "Target.h"

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)
#import <AVFoundation/AVFoundation.h>
#import <Foundation/Foundation.h>
#endif

#include <mutex>
NS_GPUPIXEL_BEGIN

typedef std::function<
    void(const uint8_t* data, int width, int height, int64_t ts)>
    RawOutputCallback;
#define PBO_SIZE 2
class TargetRawDataOutput : public Target {
 public:
  TargetRawDataOutput();
  virtual ~TargetRawDataOutput();
  static std::shared_ptr<TargetRawDataOutput> create();
  void update(int64_t frameTime) override;
  void setI420Callbck(RawOutputCallback cb);
  void setPixelsCallbck(RawOutputCallback cb);
  void SetCurrentFrameInvalid();  // when re_enable_ is true, do not deliver the
                                  // frame, then set it false.
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
  GLProgram* _filterProgram;
  GLuint _filterPositionAttribute;
  GLuint _filterTexCoordAttribute;
  //
#if defined(GPUPIXEL_IOS)
  GLuint _framebuffer = 0;
  CVOpenGLESTextureRef renderTexture = NULL;
  CVOpenGLESTextureCacheRef textureCache = NULL;
  CVPixelBufferRef renderTarget = NULL;
#else
  std::shared_ptr<Framebuffer> _framebuffer;
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

NS_GPUPIXEL_END
