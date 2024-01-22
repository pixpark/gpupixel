//
//  NebulaMetalView.h
//  zp-sdk-avmodule
//
//  Created by caozijun on 2023/2/27.
//

#import <Cocoa/Cocoa.h>

NS_ASSUME_NONNULL_BEGIN

// 渲染画面填充模式。
typedef NS_ENUM(NSInteger, NebulaMetalViewContentMode) {
  // 自动填充满，可能会变形。
  NebulaMetalViewContentModeStretch = 0,
  // 按比例适配，可能会有黑边。
  NebulaMetalViewContentModeFit = 1,
  // 根据比例裁剪后填充满。
  NebulaMetalViewContentModeFill = 2
};

typedef NS_ENUM(NSInteger, NebulaMetalViewRotation) {
  NebulaMetalViewRotation0 = 0,
  NebulaMetalViewRotation90 = 90,
  NebulaMetalViewRotation180 = 180,
  NebulaMetalViewRotation270 = 270
};

@interface NebulaMetalView : NSView
@property(nonatomic, assign)
    NebulaMetalViewContentMode fillMode; // 画面填充模式。
@property(nonatomic, assign) NebulaMetalViewRotation rotation;
@property(nonatomic, assign) BOOL mirror;
- (void)renderPixelBuffer:(CVPixelBufferRef)pixelBuffer; // 渲染。
@end

NS_ASSUME_NONNULL_END
