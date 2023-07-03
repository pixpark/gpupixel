/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#ifndef BEAUTY_VIDEO_VIEW_H
#define BEAUTY_VIDEO_VIEW_H

#ifdef __APPLE__
#import <TargetConditionals.h>
#endif

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif

#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
@interface VideoPreview : UIView
#else
@interface BeautyFaceVideoView : NSView
#endif
- (void)switchBeautyFace:(bool)state;

- (void)renderRGBA:(int)width
        withHeight:(int)height
        withStride:(int)stride
        withPixels:(const uint8_t*)pixels;

- (void)renderNV12:(const uint8_t*)bufferY
       withStrideY:(int)strideY
        withDataUV:(const uint8_t*)uvBuffer
      withStrideUV:(int)strideUV
         withWidth:(int)width
        withHeight:(int)height;


- (void)setFaceSmoothLevel:(float)level;
- (void)setWhitenLevel:(float)level;
- (void)setFaceSlimLevel:(float)level;
- (void)setEyeZoomLevel:(float)level;
@end

#endif /* BEAUTY_VIDEO_VIEW_H */
