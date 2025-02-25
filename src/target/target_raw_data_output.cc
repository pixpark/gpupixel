//
//  SourceRawOutput.cc
//  GPUPixel iOS
//
//  Created by PixPark on 2021/6/24.
//  Copyright © 2021 PixPark. All rights reserved.
//

#include "target_raw_data_output.h"
#include "gpupixel_context.h"
#include <cstring>
#include "libyuv.h"
USING_NS_GPUPIXEL

const std::string kRGBToI420VertexShaderString = R"(
    attribute vec4 position; attribute vec4 inputTextureCoordinate;
    varying vec2 textureCoordinate;

    void main() {
      gl_Position = position;
      textureCoordinate = inputTextureCoordinate.xy;
    })";

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_ANDROID)
const std::string kRGBToI420FragmentShaderString = R"(
    varying mediump vec2 textureCoordinate; uniform sampler2D sTexture;
    void main() { gl_FragColor = texture2D(sTexture, textureCoordinate); })";

#elif defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
const std::string kRGBToI420FragmentShaderString = R"(
    precision mediump float;
    varying vec2 textureCoordinate; uniform sampler2D sTexture;

    void main() { gl_FragColor = texture2D(sTexture, textureCoordinate); })";
#endif

std::shared_ptr<TargetRawDataOutput> TargetRawDataOutput::create() {
  auto sourceRawDataOutput =
      std::shared_ptr<TargetRawDataOutput>(new TargetRawDataOutput());
  return sourceRawDataOutput;
}

TargetRawDataOutput::TargetRawDataOutput() {
  initWithShaderString(kRGBToI420VertexShaderString,
                       kRGBToI420FragmentShaderString);
}

TargetRawDataOutput::~TargetRawDataOutput() {
  // delete rgba frame buffer
  if (_readPixelData != nullptr) {
    delete[] _readPixelData;
  }
  _readPixelData = nullptr;

  // delete yuv frame buffer
  if (_yuvFrameBuffer != nullptr) {
    delete[] _yuvFrameBuffer;
  }
  _yuvFrameBuffer = nullptr;

  gpupixel::GPUPixelContext::getInstance()->runSync([=] {
#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
    if (_framebuffer) {
      CHECK_GL(glDeleteFramebuffers(1, &_framebuffer));
      _framebuffer = 0;
    }

    if (renderTarget) {
      CFRelease(renderTarget);
      renderTarget = NULL;
    }

    if (renderTexture) {
      CFRelease(renderTexture);
      renderTexture = NULL;
    }

    if (textureCache) {
      CFRelease(textureCache);
      textureCache = NULL;
    }
#endif

    CHECK_GL(glDeleteBuffers(PBO_SIZE, pboIds));

    CHECK_GL(glDeleteBuffers(PBO_SIZE, pboIds_yuvdata));
  });
}

// override
void TargetRawDataOutput::update(int64_t frameTime) {
  if (_inputFramebuffers.empty()) {
    return;
  }

  int width = _inputFramebuffers[0].frameBuffer->getWidth();
  int height = _inputFramebuffers[0].frameBuffer->getHeight();
  if (_width != width || _height != height) {
    _width = width;
    _height = height;
#if defined(GPUPIXEL_IOS)
    initTextureCache(width, height);
#else
    initPBO(width, height);
    initFrameBuffer(width, height);
#endif
    initOutputBuffer(width, height);
  }
  _frame_ts = frameTime;
  renderToOutput();
}

bool TargetRawDataOutput::initWithShaderString(
    const std::string& vertexShaderSource,
    const std::string& fragmentShaderSource) {
  _filterProgram =
      GLProgram::createByShaderString(vertexShaderSource, fragmentShaderSource);
  GPUPixelContext::getInstance()->setActiveShaderProgram(_filterProgram);
  _filterPositionAttribute = _filterProgram->getAttribLocation("position");
  _filterTexCoordAttribute =
      _filterProgram->getAttribLocation("inputTextureCoordinate");

  return true;
}

int TargetRawDataOutput::renderToOutput() {
  GPUPixelContext::getInstance()->setActiveShaderProgram(_filterProgram);
#if defined(GPUPIXEL_IOS)
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer));
#else
  _framebuffer->active();
#endif

  CHECK_GL(glViewport(0, 0, _width, _height));

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLfloat imageVertices[] = {
      -1.0, -1.0,  // left down
      1.0,  -1.0,  // right down
      -1.0, 1.0,   // left up
      1.0,  1.0    // right up
  };

  GLfloat textureVertices[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  CHECK_GL(glEnableVertexAttribArray(_filterPositionAttribute));
  CHECK_GL(glVertexAttribPointer(_filterPositionAttribute, 2, GL_FLOAT, 0, 0,
                                 imageVertices));

  CHECK_GL(glEnableVertexAttribArray(_filterTexCoordAttribute));
  CHECK_GL(glVertexAttribPointer(_filterTexCoordAttribute, 2, GL_FLOAT, 0, 0,
                                 textureVertices));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _inputFramebuffers[0].frameBuffer->getTexture());

  CHECK_GL(_filterProgram->setUniformValue("sTexture", 0));
  // draw frame buffer
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
#if defined(GPUPIXEL_IOS)
  readPixelsFromCVPixelBuffer();
#else
  // read with pbo
  readPixelsWithPBO(_width, _height);
  _framebuffer->inactive();
#endif
  return 0;
}

void TargetRawDataOutput::setI420Callbck(RawOutputCallback cb) {
  std::unique_lock<std::mutex> lck(mtx_);
  i420_callback_ = cb;
}

void TargetRawDataOutput::setPixelsCallbck(RawOutputCallback cb) {
  std::unique_lock<std::mutex> lck(mtx_);
  pixels_callback_ = cb;
}

void TargetRawDataOutput::initOutputBuffer(int width, int height) {
  uint32_t rgb_size = width * height * 4;
  uint32_t yuv_size = width * height * 3 / 2;

  // alloc rgba frame buffer
  if (_readPixelData != nullptr) {
    delete[] _readPixelData;
  }
  _readPixelData = new uint8_t[rgb_size];
  std::memset(_readPixelData, 0, rgb_size);
  // alloc yuv frame buffer
  if (_yuvFrameBuffer != nullptr) {
    delete[] _yuvFrameBuffer;
  }
  _yuvFrameBuffer = new uint8_t[yuv_size];
  std::memset(_yuvFrameBuffer, 0, yuv_size);
}

#if defined(GPUPIXEL_IOS)
void TargetRawDataOutput::readPixelsFromCVPixelBuffer() {
  if (kCVReturnSuccess ==
      CVPixelBufferLockBaseAddress(renderTarget, kCVPixelBufferLock_ReadOnly)) {
    int stride = static_cast<int>(CVPixelBufferGetBytesPerRow(renderTarget));
    uint8_t* pixels = (uint8_t*)CVPixelBufferGetBaseAddress(renderTarget);

    // process pixels how you like
    if (pixels && i420_callback_) {
      libyuv::ARGBToI420(pixels, stride, _yuvFrameBuffer, _width,
                        _yuvFrameBuffer + _width * _height, _width
                        / 2, _yuvFrameBuffer + _width * _height * 5
                        / 4, _width / 2, _width, _height);
      i420_callback_(_yuvFrameBuffer, _width, _height, _frame_ts);
    }

    if(pixels_callback_) {
      pixels_callback_(pixels, _width, _height, _frame_ts);
    }

    CVPixelBufferUnlockBaseAddress(renderTarget, kCVPixelBufferLock_ReadOnly);
  } 
}

void TargetRawDataOutput::initTextureCache(int width, int height) {
  CFDictionaryRef empty;  // empty value for attr value.
  CFMutableDictionaryRef attrs;
  empty = CFDictionaryCreate(
      kCFAllocatorDefault,  // our empty IOSurface properties dictionary
      NULL, NULL, 0, &kCFTypeDictionaryKeyCallBacks,
      &kCFTypeDictionaryValueCallBacks);
  attrs = CFDictionaryCreateMutable(kCFAllocatorDefault, 1,
                                    &kCFTypeDictionaryKeyCallBacks,
                                    &kCFTypeDictionaryValueCallBacks);

  CFDictionarySetValue(attrs, kCVPixelBufferIOSurfacePropertiesKey, empty);

  // ----------- step2
  // for simplicity, lets just say the image width & height
  CVPixelBufferCreate(kCFAllocatorDefault, _width, _height,
                      kCVPixelFormatType_32BGRA, attrs, &renderTarget);

  // in real life check the error return value of course.
  if (textureCache == NULL) {
    CVReturn err = CVOpenGLESTextureCacheCreate(
        kCFAllocatorDefault, NULL,
        GPUPixelContext::getInstance()->getEglContext(), NULL, &textureCache);

    if (err) {
      // todo(Jeayo)
    }
  }

  // first create a texture from our renderTarget
  // textureCache will be what you previously made with
  // CVOpenGLESTextureCacheCreate
  CVReturn err = CVOpenGLESTextureCacheCreateTextureFromImage(
      kCFAllocatorDefault, textureCache, renderTarget,
      NULL,  // texture attributes
      GL_TEXTURE_2D,
      GL_RGBA,  // opengl format
      _width, _height,
      GL_BGRA,  // native iOS format
      GL_UNSIGNED_BYTE, 0, &renderTexture);
  if (err) {
    // todo(Jeayo)
  }

  CFRelease(attrs);
  CFRelease(empty);

  // set the texture up like any other texture
  CHECK_GL(glBindTexture(CVOpenGLESTextureGetTarget(renderTexture),
                         CVOpenGLESTextureGetName(renderTexture)));

  CHECK_GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
  CHECK_GL(glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

  CHECK_GL(glGenFramebuffers(1, &_framebuffer));
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer));

  CHECK_GL(glBindTexture(CVOpenGLESTextureGetTarget(renderTexture),
                         CVOpenGLESTextureGetName(renderTexture)));

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         CVOpenGLESTextureGetName(renderTexture), 0);

  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}
#else
void TargetRawDataOutput::initFrameBuffer(int width, int height) {
  if (!_framebuffer || (_framebuffer->getWidth() != width ||
                        _framebuffer->getHeight() != height)) {
    _framebuffer =
        GPUPixelContext::getInstance()->getFramebufferCache()->fetchFramebuffer(
            width, height);
  }
}

void TargetRawDataOutput::initPBO(int width, int height) {
  CHECK_GL(glGenBuffers(PBO_SIZE, pboIds));
  for (int i = 0; i < PBO_SIZE; ++i) {
    CHECK_GL(glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[i]));
    CHECK_GL(glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4, 0,
                          GL_STREAM_READ));
  }
  CHECK_GL(glBindBuffer(GL_PIXEL_PACK_BUFFER, 0));
}

// read pixel with pbo
void TargetRawDataOutput::readPixelsWithPBO(int width, int height) {
  index = (index + 1) % 2;
  nextIndex = (index + 1) % 2;

  // read pixels from framebuffer to PBO
  // glReadPixels() should return immediately.
  CHECK_GL(glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[index]));
  CHECK_GL(glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, 0));
  // map the PBO to process its data by CPU
  CHECK_GL(glBindBuffer(GL_PIXEL_PACK_BUFFER, pboIds[nextIndex]));

#if defined(GPUPIXEL_MAC) || defined(GPUPIXEL_WIN) || (defined(GPUPIXEL_LINUX) && !defined(__emscripten__))
  GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
#elif defined(GPUPIXEL_ANDROID)
  GLubyte* ptr = (GLubyte*)glMapBufferRange(
                  GL_PIXEL_PACK_BUFFER, 0, width * height * 4, GL_MAP_READ_BIT);
#else
  GLubyte* ptr = nullptr;
#endif
  if (ptr) {
       libyuv::ABGRToI420(ptr, width * 4, _yuvFrameBuffer, _width,
                          _yuvFrameBuffer + _width * _height, _width / 2,
                          _yuvFrameBuffer + _width * _height * 5 / 4, _width
                          / 2, _width, _height);
    if (i420_callback_) {
      i420_callback_(_yuvFrameBuffer, _width, _height, _frame_ts);
    }

    if(pixels_callback_) {
      pixels_callback_(ptr, _width, _height, _frame_ts);
    }

    glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
  }
  glBindBuffer(GL_PIXEL_PACK_BUFFER, GL_NONE);
}

#ifdef __emscripten__
EMSCRIPTEN_BINDINGS(target_raw_data_output) {
  emscripten::class_<TargetRawDataOutput, emscripten::base<Target>>("TargetRawDataOutput")
  .constructor<>()
  .smart_ptr<std::shared_ptr<TargetRawDataOutput>>("TargetRawDataOutput")
  .class_function("create", &TargetRawDataOutput::create, emscripten::return_value_policy::take_ownership());

}
#endif

#endif
