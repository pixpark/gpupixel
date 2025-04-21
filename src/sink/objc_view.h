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

#include "gpupixel/sink/sink_render.h"

#if defined(GPUPIXEL_IOS)
GPUPIXEL_API
@interface ObjcView : UIView

@property(nonatomic) CAEAGLLayer* currentlayer;
@property(nonatomic) CGRect currentFrame;

#else
GPUPIXEL_API
@interface ObjcView : NSOpenGLView
#endif
@property(readwrite, nonatomic) gpupixel::SinkRender::FillMode fillMode;
@property(readonly, nonatomic) CGSize sizeInPixels;

// 直接实现方法而不是通过协议
- (void)DoRender;
- (void)SetInputFramebuffer:(std::shared_ptr<gpupixel::GPUPixelFramebuffer>)framebuffer 
               withRotation:(gpupixel::RotationMode)rotationMode 
                    atIndex:(int)textureIndex;
- (BOOL)IsReady;
- (void)unPrepared;

@end
