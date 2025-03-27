/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#ifdef __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_IPHONE
        #define GPUPIXEL_IOS 1
    #elif TARGET_OS_MAC
        #define GPUPIXEL_MAC 1
    #endif
#endif

#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/NSOpenGLView.h>
#endif

#include <memory>
#include "sink.h"
#include "sink_render.h"

namespace gpupixel {
    class GPUPixelFramebuffer;
}

@protocol GPUPixelSink <NSObject>
- (void)setInputFramebuffer:(std::shared_ptr<gpupixel::GPUPixelFramebuffer>)newInputFramebuffer
               withRotation:(gpupixel::RotationMode)rotation
                    atIndex:(NSInteger)texIdx;
- (void)doRender;
@end

#if defined(GPUPIXEL_IOS)
@interface GPUPixelView : UIView <GPUPixelSink>
#else
@interface GPUPixelView : NSOpenGLView <GPUPixelSink>
#endif
@property(readwrite, nonatomic) gpupixel::SinkRender::FillMode fillMode;
@property(readonly, nonatomic) CGSize sizeInPixels;
@end
