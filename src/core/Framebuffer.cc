/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "framebuffer.h"
#include <assert.h>
#include <algorithm>
#include "gpupixel_context.h"
#include "util.h"

NS_GPUPIXEL_BEGIN

// std::vector<std::shared_ptr<Framebuffer>> Framebuffer::_framebuffers;
#ifndef GPUPIXEL_WIN
TextureAttributes Framebuffer::defaultTextureAttribures = {
    .minFilter = GL_LINEAR,
    .magFilter = GL_LINEAR,
    .wrapS = GL_CLAMP_TO_EDGE,
    .wrapT = GL_CLAMP_TO_EDGE,
    .internalFormat = GL_RGBA,
    .format = GL_RGBA,
    .type = GL_UNSIGNED_BYTE};
#else
TextureAttributes Framebuffer::defaultTextureAttribures = {
    GL_LINEAR, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE,
    GL_RGBA,   GL_RGBA,   GL_UNSIGNED_BYTE};
#endif
Framebuffer::Framebuffer(
    int width,
    int height,
    bool onlyGenerateTexture /* = false*/,
    const TextureAttributes textureAttributes /* = defaultTextureAttribures*/)
    : _texture(-1), _framebuffer(-1) {
  _width = width;
  _height = height;
  _textureAttributes = textureAttributes;
  _hasFB = !onlyGenerateTexture;

  if (_hasFB) {
    _generateFramebuffer();
  } else {
    _generateTexture();
  }
}

Framebuffer::~Framebuffer() {
  gpupixel::GPUPixelContext::getInstance()->runSync([&] {
    bool bDeleteTex = (_texture != -1);
    bool bDeleteFB = (_framebuffer != -1);

    if (bDeleteTex) {
      CHECK_GL(glDeleteTextures(1, &_texture));
      _texture = -1;
    }
    if (bDeleteFB) {
      CHECK_GL(glDeleteFramebuffers(1, &_framebuffer));
      _framebuffer = -1;
    }
  });
}

void Framebuffer::active() {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer));
  CHECK_GL(glViewport(0, 0, _width, _height));
}

void Framebuffer::inactive() {
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void Framebuffer::_generateTexture() {
  CHECK_GL(glGenTextures(1, &_texture));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, _texture));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                           _textureAttributes.minFilter));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,
                           _textureAttributes.magFilter));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                           _textureAttributes.wrapS));
  CHECK_GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                           _textureAttributes.wrapT));

  // TODO: Handle mipmaps
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

void Framebuffer::_generateFramebuffer() {
  CHECK_GL(glGenFramebuffers(1, &_framebuffer));
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer));
  _generateTexture();
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, _texture));
  CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, _textureAttributes.internalFormat,
                        _width, _height, 0, _textureAttributes.format,
                        _textureAttributes.type, 0));
  CHECK_GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                  GL_TEXTURE_2D, _texture, 0));
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
  CHECK_GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

NS_GPUPIXEL_END
