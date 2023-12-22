/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#pragma once

#include "Source.h"

NS_GPUPIXEL_BEGIN

class SourceImage : public Source {
 public:
  SourceImage() {}
  ~SourceImage();
  static std::shared_ptr<SourceImage> create(int width,
                                             int height,
                                             const void* pixels);
  static std::shared_ptr<SourceImage> create(const std::string name);
  SourceImage* setImage(int width, int height, const void* pixels);

#if defined(GPUPIXEL_IOS)
  static std::shared_ptr<SourceImage> create(NSURL* imageUrl);
  SourceImage* setImage(NSURL* imageUrl);

  static std::shared_ptr<SourceImage> create(NSData* imageData);
  SourceImage* setImage(NSData* imageData);

  static std::shared_ptr<SourceImage> create(UIImage* image);
  void setImage(UIImage* image);

  static std::shared_ptr<SourceImage> create(CGImageRef image);
  SourceImage* setImage(CGImageRef image);
#endif

 private:
#if defined(GPUPIXEL_IOS)
  UIImage* _adjustImageOrientation(UIImage* image);
#endif
#if defined(GPUPIXEL_ANDROID)
    static std::shared_ptr<SourceImage> createImageForAndroid(std::string name);
#endif
};

NS_GPUPIXEL_END
