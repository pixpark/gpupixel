/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "gpupixel_define.h"

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

  GLuint GetTexture() const { return _texture; }

  GLuint GetFramebuffer() const { return _framebuffer; }

  int GetWidth() const { return _width; }
  int GetHeight() const { return _height; }
  const TextureAttributes& GetTextureAttributes() const {
    return _textureAttributes;
  };
  bool HasFramebuffer() { return _hasFB; };

  void Active();
  void Inactive();

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
