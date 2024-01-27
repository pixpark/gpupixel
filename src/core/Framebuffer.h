/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"

#if defined(GPUPIXEL_IOS)
#import <OpenGLES/ES3/gl.h>
#import <OpenGLES/ES3/glext.h>
#elif defined(GPUPIXEL_MAC)
#import <OpenGL/gl3.h>
#elif defined(GPUPIXEL_ANDROID)
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#elif defined(GPUPIXEL_WIN) || defined(GPUPIXEL_LINUX)
  #define GL_GLEXT_PROTOTYPES
  #define GLEW_STATIC
  #define GLFW_INCLUDE_GLCOREARB
  #include <GLFW/glfw3.h>
#endif

#include <vector>

NS_GPUPIXEL_BEGIN

typedef struct {
  GLenum minFilter;
  GLenum magFilter;
  GLenum wrapS;
  GLenum wrapT;
  GLenum internalFormat;
  GLenum format;
  GLenum type;
} TextureAttributes;

class Framebuffer {
 public:
  Framebuffer(
      int width,
      int height,
      bool onlyGenerateTexture = false,
      const TextureAttributes textureAttributes = defaultTextureAttribures);
  ~Framebuffer();

  GLuint getTexture() const { return _texture; }

  GLuint getFramebuffer() const { return _framebuffer; }

  int getWidth() const { return _width; }
  int getHeight() const { return _height; }
  const TextureAttributes& getTextureAttributes() const {
    return _textureAttributes;
  };
  bool hasFramebuffer() { return _hasFB; };

  void active();
  void inactive();

  static TextureAttributes defaultTextureAttribures;

 private:
  int _width, _height;
  TextureAttributes _textureAttributes;
  bool _hasFB;
  GLuint _texture;
  GLuint _framebuffer;

  void _generateTexture();
  void _generateFramebuffer();

  //    static std::vector<std::shared_ptr<Framebuffer>> _framebuffers;
};

NS_GPUPIXEL_END
