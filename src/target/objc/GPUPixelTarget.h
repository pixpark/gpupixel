/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */
#include "GPUPixelDef.h"
#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#elif defined(GPUPIXEL_MAC)
#import <AppKit/AppKit.h>
#endif

#if defined(GPUPIXEL_IOS) || defined(GPUPIXEL_MAC)

#include "Framebuffer.h"
#include "Target.h"

@protocol GPUPixelTarget <NSObject>

@required
- (void)update:(float)frameTime;
- (void)setInputFramebuffer:
            (std::shared_ptr<GPUPixel::Framebuffer>)inputFramebuffer
               withRotation:(GPUPixel::RotationMode)rotationMode
                    atIndex:(NSInteger)texIdx;
@optional
- (bool)isPrepared;
- (void)unPrepared;

@end

#endif
