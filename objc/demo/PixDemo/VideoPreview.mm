/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "VideoPreview.h"
#include <GPUPixel/GPUPixel.h>
 
using namespace GPUPixel;
 
@interface VideoPreview () {
  std::shared_ptr<SourceRawDataInput> gpuPixelRawInput;
  GPUPixelView *gpuPixelView;
  std::shared_ptr<FaceBeautyFilter> beauty_face_filter_;
  std::shared_ptr<TargetRawDataOutput> targetRawOutput_;
  std::shared_ptr<FaceReshapeFilter> face_reshape_filter_;
}

@end

@implementation VideoPreview

- (id)initWithFrame:(CGRect)frame {
  if (![super initWithFrame:frame]) {
    return nil;
  }
  [self setBackgroundColor:[UIColor blackColor]];
  GPUPixel::GPUPixelContext::getInstance()->runSync([&] {
    gpuPixelRawInput = SourceRawDataInput::create();
    gpuPixelView = [[GPUPixelView alloc] initWithFrame:frame];
    [self addSubview:gpuPixelView];
    
    // create filter
    targetRawOutput_ = TargetRawDataOutput::create();
    beauty_face_filter_ = FaceBeautyFilter::create();
    
    // filter pipline
    gpuPixelRawInput->addTarget(beauty_face_filter_);
    beauty_face_filter_->addTarget(gpuPixelView);
  });
  return self;
}

- (void)switchBeautyFace:(bool)state {
  gpuPixelRawInput->removeAllTargets();
  beauty_face_filter_->removeAllTargets();
  if (state) {
      gpuPixelRawInput->addTarget(beauty_face_filter_);
      beauty_face_filter_->addTarget(gpuPixelView);
  } else {
    gpuPixelRawInput->addTarget(gpuPixelView);
  }
}

- (void)renderRGBA:(int)width
        withHeight:(int)height
        withStride:(int)stride
        withPixels:(const uint8_t *)pixels {
    gpuPixelRawInput->uploadBytes(pixels, width, height, stride);
}

- (void)renderNV12:(const uint8_t *)bufferY
       withStrideY:(int)strideY
        withDataUV:(const uint8_t *)uvBuffer
      withStrideUV:(int)strideUV
         withWidth:(int)width
        withHeight:(int)height {
  // todo(Jeayo)
}

- (void)setFaceSmoothLevel:(float)level {
  beauty_face_filter_->setBlurAlpha(level / 10);  //[0.0, 1.0]
}

- (void)setWhitenLevel:(float)level {
  beauty_face_filter_->setWhite(level / 30);  // [0.0, 0.01]
}


- (void)setFaceSlimLevel:(float)level {

}

- (void)setEyeZoomLevel:(float)level {
    
}
@end
