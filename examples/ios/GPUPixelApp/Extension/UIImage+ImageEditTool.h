//
//  UIImage+ImageEditTool.h
//  GPUPixelApp
//
//  Created by Admin on 2/6/2025.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@interface UIImage (ImageEditTool)
/// 将后置摄像头的照片进行UP方向矫正
- (UIImage *)upOrientation;
@end

NS_ASSUME_NONNULL_END
