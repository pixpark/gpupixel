/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_macros.h"

#include <vector>

namespace gpupixel {
GPUPIXEL_API typedef struct {
  GLenum minFilter;
  GLenum magFilter;
  GLenum wrapS;
  GLenum wrapT;
  GLenum internalFormat;
  GLenum format;
  GLenum type;
} TextureAttributes;

class GPUPIXEL_API GPUPixelFramebuffer {
 public:
  GPUPixelFramebuffer(
      int width,
      int height,
      bool onlyGenerateTexture = false,
      const TextureAttributes textureAttributes = defaultTextureAttribures);
  ~GPUPixelFramebuffer();

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

  //    static std::vector<std::shared_ptr<GPUPixelFramebuffer>> _framebuffers;
};

}
