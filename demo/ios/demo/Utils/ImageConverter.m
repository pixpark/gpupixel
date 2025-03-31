//
//  ImageConverter.m
//  demo
//
//  Created by WenYu on 3/28/25.
//

#import "ImageConverter.h"

@implementation ImageConverter

+ (UIImage *)imageFromData:(const uint8_t *)data width:(int)width height:(int)height bitmapInfo:(uint32_t)bitmapInfo
{
    size_t dataSize = width * height * 4;
    unsigned char *imageData = (unsigned char *)malloc(dataSize);
    if (!imageData) {
        NSLog(@"内存分配失败");
        return nil;
    }
    memcpy(imageData, data, dataSize);

    // 设置图像参数
    size_t bitsPerComponent = 8; // r g b a 每个component bits数目
    size_t bytesPerRow = width * 4; // 一张图片每行字节数目(每个像素点包含 r g b a 四个字节)
    CGColorSpaceRef space = CGColorSpaceCreateDeviceRGB(); // 创建 rgb 颜色空间

    // 创建图片上下文和图像
    CGContextRef context = CGBitmapContextCreate(imageData, width, height, bitsPerComponent, bytesPerRow, space, bitmapInfo);
    CGImageRef cgImage = CGBitmapContextCreateImage(context);
    UIImage *resultImage = [UIImage imageWithCGImage:cgImage];

    // 释放内存
    free(imageData);
    CGColorSpaceRelease(space);
    CGContextRelease(context);
    CGImageRelease(cgImage);

    return resultImage;
}

+ (UIImage *)imageFromARGBData:(const uint8_t *)data width:(int)width height:(int)height
{
    uint32_t bitmapInfo = kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little;
    return [self imageFromData:data width:width height:height bitmapInfo:bitmapInfo];
}

+ (UIImage *)imageFromRGBAData:(const uint8_t *)data width:(int)width height:(int)height
{
    uint32_t bitmapInfo = kCGImageAlphaPremultipliedLast | kCGImageByteOrder32Big;
    return [self imageFromData:data width:width height:height bitmapInfo:bitmapInfo];
}

@end
