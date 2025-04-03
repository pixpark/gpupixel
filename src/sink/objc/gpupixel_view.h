/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel_define.h"
#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/NSOpenGLView.h>
#endif

#import "gpupixel_sink.h"
#include "sink_render.h"

#if defined(GPUPIXEL_IOS)
@interface GPUPixelView : UIView <GPUPixelSink>
#else
@interface GPUPixelView : NSOpenGLView <GPUPixelSink>
#endif
@property(readwrite, nonatomic) gpupixel::SinkRender::FillMode fillMode;
@property(readonly, nonatomic) CGSize sizeInPixels;
@end
