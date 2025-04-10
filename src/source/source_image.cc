/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/source/source_image.h"
#include "core/gpupixel_context.h"
#include "utils/util.h"

#if defined(GPUPIXEL_ANDROID)
#include <android/bitmap.h>
#include "android/jni/jni_helpers.h"
#endif

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

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

std::shared_ptr<SourceImage> SourceImage::Create(const std::string name) {
  int width, height, channel_count;
  unsigned char* data =
      stbi_load(name.c_str(), &width, &height, &channel_count, 4);
  //   todo(logo info)
  if (data == nullptr) {
    Util::Log("SourceImage", "SourceImage: input data in null! file name: %s",
              name.c_str());
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
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, this->GetFramebuffer()->GetTexture()));

  CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                        GL_UNSIGNED_BYTE, pixels));
  image_bytes_.assign(pixels, pixels + width * height * 4);

  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
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
