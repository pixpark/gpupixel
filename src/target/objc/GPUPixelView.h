/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "GPUPixelDef.h"
 
#import <AppKit/NSOpenGLView.h>
  
#import "GPUPixelTarget.h"
#include "TargetView.h"

 
@interface GPUPixelView : NSOpenGLView <GPUPixelTarget>
@property(readwrite, nonatomic) GPUPixel::TargetView::FillMode fillMode;
@property(readonly, nonatomic) CGSize sizeInPixels;
@end
