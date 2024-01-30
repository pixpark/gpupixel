/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel_macros.h"
#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/NSOpenGLView.h>
#endif

#import "gpupixel_target.h"
#include "target_view.h"

#if defined(GPUPIXEL_IOS)
@interface GPUPixelView : UIView <GPUPixelTarget>
#else
@interface GPUPixelView : NSOpenGLView <GPUPixelTarget>
#endif
@property(readwrite, nonatomic) gpupixel::TargetView::FillMode fillMode;
@property(readonly, nonatomic) CGSize sizeInPixels;
@end
