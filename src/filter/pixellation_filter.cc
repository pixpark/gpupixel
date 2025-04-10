/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/pixellation_filter.h"
#include "core/gpupixel_context.h"
namespace gpupixel {

const std::string kPixellationFragmentShaderString = R"(
    uniform highp float pixelSize; uniform highp float aspectRatio;

    uniform sampler2D inputImageTexture;
    varying highp vec2 textureCoordinate;

    void main() {
      highp vec2 pixelSizeVec = vec2(pixelSize, pixelSize / aspectRatio);
      highp vec2 samplePos =
          floor(textureCoordinate / pixelSizeVec) * pixelSizeVec +
          0.5 * pixelSizeVec;
      gl_FragColor = texture2D(inputImageTexture, samplePos);
    })";

std::shared_ptr<PixellationFilter> PixellationFilter::Create() {
  auto ret = std::shared_ptr<PixellationFilter>(new PixellationFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool PixellationFilter::Init() {
  if (!InitWithFragmentShaderString(kPixellationFragmentShaderString)) {
    return false;
  }

  pixel_size_ = 0.1;
  RegisterProperty(
      "pixelSize", pixel_size_,
      "The size of a pixel that you want to pixellate, ranges from 0 to 1.",
      [this](float& pixelSize) { setPixelSize(pixelSize); });

  return true;
}

void PixellationFilter::setPixelSize(float pixelSize) {
  pixel_size_ = pixelSize;
  if (pixel_size_ > 1.0) {
    pixel_size_ = 1.0;
  } else if (pixel_size_ < 0.0) {
    pixel_size_ = 0.0;
  }
}

bool PixellationFilter::DoRender(bool updateSinks) {
  float aspectRatio = 1.0;
  std::shared_ptr<GPUPixelFramebuffer> firstInputFramebuffer =
      input_framebuffers_.begin()->second.frame_buffer;
  aspectRatio = firstInputFramebuffer->GetHeight() /
                (float)(firstInputFramebuffer->GetWidth());
  filter_program_->SetUniformValue("aspectRatio", aspectRatio);

  float pixelSize = pixel_size_;
  float singlePixelWidth = 1.0 / firstInputFramebuffer->GetWidth();
  if (pixelSize < singlePixelWidth) {
    pixelSize = singlePixelWidth;
  }
  filter_program_->SetUniformValue("pixelSize", pixelSize);

  return Filter::DoRender(updateSinks);
}

}  // namespace gpupixel
