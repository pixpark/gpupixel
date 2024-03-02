/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */
#include "gpupixel_macros.h"
#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#elif defined(GPUPIXEL_MAC)
#import <AppKit/AppKit.h>
#endif

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)

#include "framebuffer.h"
#include "target.h"

@protocol GPUPixelTarget <NSObject>

@required
- (void)update:(float)frameTime;
- (void)setInputFramebuffer:
            (std::shared_ptr<gpupixel::Framebuffer>)inputFramebuffer
               withRotation:(gpupixel::RotationMode)rotationMode
                    atIndex:(NSInteger)texIdx;
@optional
- (bool)isPrepared;
- (void)unPrepared;

@end

#endif
