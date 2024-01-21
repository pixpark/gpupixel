/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "GPUPixelDef.h"
#if defined(GPUPIXEL_IOS)
#import <UIKit/UIKit.h>
#else
#import <AppKit/NSOpenGLView.h>
#endif

#import "GPUPixelTarget.h"
#include "TargetView.h"

#if defined(GPUPIXEL_IOS)
@interface GPUPixelView : UIView <GPUPixelTarget>
#else
@interface GPUPixelView : NSOpenGLView <GPUPixelTarget>
#endif
@property(readwrite, nonatomic) GPUPixel::TargetView::FillMode fillMode;
@property(readonly, nonatomic) CGSize sizeInPixels;
@end
