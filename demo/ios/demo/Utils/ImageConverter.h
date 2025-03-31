//
//  ImageConverter.h
//  demo
//
//  Created by WenYu on 3/28/25.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

/**
 * 图像转换工具类
 * 提供将原始图像数据转换为UIImage的方法
 */
@interface ImageConverter : NSObject

/// 转换一个基于给定数据、宽度、高度和位图信息的图像
/// - Parameters:
///   - data: 原始图像数据指针
///   - width: 图像宽度
///   - height: 图像高度
///   - bitmapInfo: 位图信息
+ (UIImage *)imageFromData:(const uint8_t *)data width:(int)width height:(int)height bitmapInfo:(uint32_t)bitmapInfo;

/// 将 ARGB 格式的原始图像数据转换为 UIImage
/// - Parameters:
///   - data: 原始图像数据指针
///   - width: 图像宽度
///   - height: 图像高度
+ (UIImage *)imageFromARGBData:(const uint8_t *)data width:(int)width height:(int)height;

/// 将 RGBA 格式的原始图像数据转换为 UIImage
/// - Parameters:
///   - data: 原始图像数据指针
///   - width: 图像宽度
///   - height: 图像高度
+ (UIImage *)imageFromRGBAData:(const uint8_t *)data width:(int)width height:(int)height;

@end

NS_ASSUME_NONNULL_END
