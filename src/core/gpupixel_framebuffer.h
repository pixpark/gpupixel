/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "core/gpupixel_gl_include.h"

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
      bool only_generate_texture = false,
      const TextureAttributes texture_attributes = default_texture_attributes);
  ~GPUPixelFramebuffer();

  uint32_t GetTexture() const { return texture_; }

  uint32_t GetFramebuffer() const { return framebuffer_; }

  int GetWidth() const { return width_; }
  int GetHeight() const { return height_; }
  const TextureAttributes& GetTextureAttributes() const {
    return texture_attributes_;
  };
  bool HasFramebuffer() { return has_framebuffer_; };

  void Activate();
  void Deactivate();

  static TextureAttributes default_texture_attributes;

 private:
  int width_;
  int height_;
  TextureAttributes texture_attributes_;
  bool has_framebuffer_;
  uint32_t texture_;
  uint32_t framebuffer_;

  void GenerateTexture();
  void GenerateFramebuffer();

  //    static std::vector<std::shared_ptr<GPUPixelFramebuffer>> framebuffers_;
};

}  // namespace gpupixel
