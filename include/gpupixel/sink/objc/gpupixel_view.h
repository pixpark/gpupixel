/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/gpupixel_define.h"
#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/NSOpenGLView.h>
#endif

#import "gpupixel/sink/objc/gpupixel_sink.h"
#include "gpupixel/sink/sink_render.h"

#if defined(GPUPIXEL_IOS)
GPUPIXEL_API
@interface GPUPixelView : UIView <GPUPixelSink>

@property(nonatomic) CAEAGLLayer* currentlayer;
@property(nonatomic) CGRect currentFrame;

#else
GPUPIXEL_API
@interface GPUPixelView : NSOpenGLView <GPUPixelSink>
#endif
@property(readwrite, nonatomic) gpupixel::SinkRender::FillMode fillMode;
@property(readonly, nonatomic) CGSize sizeInPixels;

@end
