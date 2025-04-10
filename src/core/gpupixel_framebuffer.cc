/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "core/gpupixel_framebuffer.h"
#include <assert.h>
#include <algorithm>
#include "core/gpupixel_context.h"
#include "utils/util.h"

namespace gpupixel {

// std::vector<std::shared_ptr<GPUPixelFramebuffer>>
// GPUPixelFramebuffer::framebuffers_;
#ifndef GPUPIXEL_WIN
TextureAttributes GPUPixelFramebuffer::default_texture_attributes = {
    .minFilter = GL_LINEAR,
    .magFilter = GL_LINEAR,
    .wrapS = GL_CLAMP_TO_EDGE,
    .wrapT = GL_CLAMP_TO_EDGE,
    .internalFormat = GL_RGBA,
    .format = GL_RGBA,
    .type = GL_UNSIGNED_BYTE};
#else
TextureAttributes GPUPixelFramebuffer::default_texture_attributes = {
    GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
    GL_RGBA,   GL_RGBA,   GL_UNSIGNED_BYTE};
#endif
GPUPixelFramebuffer::GPUPixelFramebuffer(
    int width,
    int height,
    bool only_generate_texture /* = false*/,
    const TextureAttributes
        texture_attributes /* = default_texture_attributes*/)
    : texture_(-1), framebuffer_(-1) {
  width_ = width;
  height_ = height;
  texture_attributes_ = texture_attributes;
  has_framebuffer_ = !only_generate_texture;

  if (has_framebuffer_) {
    GenerateFramebuffer();
  } else {
    GenerateTexture();
  }
}

GPUPixelFramebuffer::~GPUPixelFramebuffer() {
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    bool should_delete_texture = (texture_ != -1);
    bool should_delete_framebuffer = (framebuffer_ != -1);

    if (should_delete_texture) {
      CHECK_GL(glDeleteTextures(1, &texture_));
      texture_ = -1;
    }
    if (should_delete_framebuffer) {
      CHECK_GL(glDeleteFramebuffers(1, &framebuffer_));
      framebuffer_ = -1;
    }
  });
}

void GPUPixelFramebuffer::Activate() {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_));
  CHECK_GL(glViewport(0, 0, width_, height_));
}

void GPUPixelFramebuffer::Deactivate() {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void GPUPixelFramebuffer::GenerateTexture() {
  CHECK_GL(glGenTextures(1, &texture_));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture_));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                           texture_attributes_.minFilter));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                           texture_attributes_.magFilter));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                           texture_attributes_.wrapS));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                           texture_attributes_.wrapT));

  // TODO: Handle mipmaps
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

void GPUPixelFramebuffer::GenerateFramebuffer() {
  CHECK_GL(glGenFramebuffers(1, &framebuffer_));
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_));
  GenerateTexture();
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, texture_));
  CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, texture_attributes_.internalFormat,
                        width_, height_, 0, texture_attributes_.format,
                        texture_attributes_.type, 0));
  CHECK_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, texture_, 0));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

}  // namespace gpupixel
