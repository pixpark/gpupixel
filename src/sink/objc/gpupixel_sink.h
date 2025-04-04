/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */
#include "gpupixel_define.h"
#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#elif defined(GPUPIXEL_MAC)
#import <AppKit/AppKit.h>
#endif

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)

#include "gpupixel_framebuffer.h"
#include "sink.h"

@protocol GPUPixelSink <NSObject>

@required
- (void)DoRender;
- (void)SetInputFramebuffer:
            (std::shared_ptr<gpupixel::GPUPixelFramebuffer>)inputFramebuffer
               withRotation:(gpupixel::RotationMode)rotation_mode
                    atIndex:(NSInteger)texIdx;
@optional
- (bool)IsReady;
- (void)unPrepared;

@end

#endif
