/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/source/source_image.h"
#include <cassert>
#include "core/gpupixel_context.h"
#include "utils/logging.h"
#include "utils/util.h"

#if defined(GPUPIXEL_ANDROID)
#include <android/bitmap.h>
#include "android/jni/jni_helpers.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace gpupixel {

std::shared_ptr<SourceImage> SourceImage::CreateFromBuffer(
    int width,
    int height,
    int channel_count,
    const unsigned char* pixels) {
  auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext(
      [&] { sourceImage->Init(width, height, channel_count, pixels); });
  return sourceImage;
}

std::shared_ptr<SourceImage> SourceImage::Create(const std::string path) {
  if (!fs::exists(path)) {
    LOG_ERROR("SourceImage: image path not found: {}", path);
    assert(false && "SourceImage: image path not found");
    return nullptr;
  }
  int width, height, channel_count;
  unsigned char* data =
      stbi_load(path.c_str(), &width, &height, &channel_count, 4);
  LOG_INFO("create source image path: {}", path);
  if (data == nullptr) {
    LOG_ERROR("stbi_load create image failed! file path: {}", path);
    assert(data != nullptr && "stbi_load create image failed");
    return nullptr;
  }
  auto image =
      SourceImage::CreateFromBuffer(width, height, channel_count, data);
  stbi_image_free(data);
  return image;
}

void SourceImage::Init(int width,
                       int height,
                       int channel_count,
                       const unsigned char* pixels) {
  this->SetFramebuffer(0);
  if (!framebuffer_ || (framebuffer_->GetWidth() != width ||
                        framebuffer_->GetHeight() != height)) {
    framebuffer_ = GPUPixelContext::GetInstance()
                       ->GetFramebufferFactory()
                       ->CreateFramebuffer(width, height, true);
  }
  this->SetFramebuffer(framebuffer_);
  GL_CALL(glBindTexture(GL_TEXTURE_2D, this->GetFramebuffer()->GetTexture()));

  GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                       GL_UNSIGNED_BYTE, pixels));
  image_bytes_.assign(pixels, pixels + width * height * 4);

  GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

void SourceImage::Render() {
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext(
      [&] { Source::DoRender(); });
}

const unsigned char* SourceImage::GetRgbaImageBuffer() const {
  return image_bytes_.data();
}

int SourceImage::GetWidth() const {
  if (framebuffer_) {
    return framebuffer_->GetWidth();
  }
  return 0;
}

int SourceImage::GetHeight() const {
  if (framebuffer_) {
    return framebuffer_->GetHeight();
  }
  return 0;
}

}  // namespace gpupixel
