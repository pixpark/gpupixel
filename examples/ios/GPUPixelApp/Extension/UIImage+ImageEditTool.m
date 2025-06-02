//
//  UIImage+ImageEditTool.m
//  GPUPixelApp
//
//  Created by Admin on 2/6/2025.
//

#import "UIImage+ImageEditTool.h"

@implementation UIImage (ImageEditTool)
/// 将后置摄像头的照片进行UP方向矫正
- (UIImage *)upOrientation {
    UIImage* inputImage = self;
    if (UIImageOrientationUp == inputImage.imageOrientation) {
        return inputImage;
    }
    CGAffineTransform transform = CGAffineTransformIdentity;
    switch (inputImage.imageOrientation) {
        case UIImageOrientationDown:
        case UIImageOrientationDownMirrored:{
            transform = CGAffineTransformTranslate(transform, inputImage.size.width, inputImage.size.height);
            transform = CGAffineTransformRotate(transform, M_PI);
        }break;
            
        case UIImageOrientationLeft:
        case UIImageOrientationLeftMirrored:{
            transform = CGAffineTransformTranslate(transform, inputImage.size.width, 0);
            transform = CGAffineTransformRotate(transform, M_PI_2);
        }break;
            
        case UIImageOrientationRight:
        case UIImageOrientationRightMirrored:{
            transform = CGAffineTransformTranslate(transform, 0, inputImage.size.height);
            transform = CGAffineTransformRotate(transform, -M_PI_2);
        }break;
            
        case UIImageOrientationUpMirrored:{
            transform = CGAffineTransformTranslate(transform, inputImage.size.width, 0);
            transform = CGAffineTransformScale(transform, -1.0, 1.0);
        }break;
            
        default:break;
    }
    
    switch (inputImage.imageOrientation) {
        case UIImageOrientationUpMirrored:
        case UIImageOrientationDownMirrored://前置摄像头[非镜像]图像方向
            transform = CGAffineTransformTranslate(transform, inputImage.size.width, 0);
            transform = CGAffineTransformScale(transform, -1.0, 1.0);
            
        case UIImageOrientationLeftMirrored:
        case UIImageOrientationRightMirrored://前置摄像头[非镜像]图像方向
            transform = CGAffineTransformTranslate(transform, inputImage.size.height, 0);
            transform = CGAffineTransformScale(transform, -1.0, 1.0);
            
        default:break;
    }
    
    CGContextRef ctx = CGBitmapContextCreate(NULL, inputImage.size.width, inputImage.size.height,
                                                  CGImageGetBitsPerComponent(inputImage.CGImage), 0,
                                                  CGImageGetColorSpace(inputImage.CGImage),
                                                  CGImageGetBitmapInfo(inputImage.CGImage));
    CGContextConcatCTM(ctx, transform);
    
    switch (inputImage.imageOrientation) {
        case UIImageOrientationLeft:
        case UIImageOrientationLeftMirrored:
        case UIImageOrientationRight:
        case UIImageOrientationRightMirrored:{
            CGContextDrawImage(ctx, CGRectMake(0, 0, inputImage.size.height, inputImage.size.width), inputImage.CGImage);
        }break;
            
        default:{
            CGContextDrawImage(ctx, CGRectMake(0, 0, inputImage.size.width, inputImage.size.height), inputImage.CGImage);
        }break;
    }
    
    CGImageRef cgImage = CGBitmapContextCreateImage(ctx);
    UIImage *outputImage = [UIImage imageWithCGImage:cgImage];
    CGContextRelease(ctx);
    CGImageRelease(cgImage);
    
    return outputImage;
}
@end
