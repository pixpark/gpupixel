/*
 * GPUPixel
 *
 * Created by gezhaoyou on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "source_image.h"
#include "gpupixel_context.h"
#include "util.h"

#if defined(GPUPIXEL_ANDROID)

#include <android/bitmap.h>

#include "jni_helpers.h"

#endif

USING_NS_GPUPIXEL

std::shared_ptr<SourceImage> SourceImage::create(int width,
                                                 int height,
                                                 const void *pixels) {
    auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
    sourceImage->setImage(width, height, pixels);
    return sourceImage;
}

std::shared_ptr<SourceImage> SourceImage::create(const std::string name) {
#if defined(GPUPIXEL_ANDROID)
    auto sourceImage = createImageForAndroid(name);
    return sourceImage;
#elif defined(GPUPIXEL_MAC)
    // Todo(Jeayo)
  auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
  auto path = Util::getResourcePath(name);
//  NSImage *image = [NSImage imageNamed:[NSString stringWithUTF8String:path.c_str()]];
  NSImage *image = [[NSImage alloc] initWithContentsOfFile:[NSString stringWithUTF8String:path.c_str()]];
  // 将 NSImage 转换为 NSBitmapImageRep
  NSBitmapImageRep *imageRep = [NSBitmapImageRep imageRepWithData:[image TIFFRepresentation]];

  // 获取图像的像素宽度和高度
  NSInteger width = [imageRep pixelsWide];
  NSInteger height = [imageRep pixelsHigh];

  // 创建一个用于存储 RGBA 数据的缓冲区
  unsigned char *rgbaBuffer = (unsigned char *)malloc(width * height * 4);

  // 提取图像的 RGBA 数据
  [imageRep getBitmapDataPlanes:&rgbaBuffer];

  // 访问 RGBA 数据
  for (NSInteger y = 0; y < height; y++) {
     for (NSInteger x = 0; x < width; x++) {
         // 计算像素在缓冲区中的索引
         NSInteger pixelIndex = (y * width + x) * 4;
         
         // 访问 RGBA 值
         unsigned char red = rgbaBuffer[pixelIndex];
         unsigned char green = rgbaBuffer[pixelIndex + 1];
         unsigned char blue = rgbaBuffer[pixelIndex + 2];
         unsigned char alpha = rgbaBuffer[pixelIndex + 3];
         
         // 在这里进行 RGBA 数据的处理或使用
         // 例如，可以打印 RGBA 值
//         NSLog(@"Pixel at (%ld, %ld): R:%hhu G:%hhu B:%hhu A:%hhu", x, y, red, green, blue, alpha);
     }
  }
  sourceImage->setImage(width, height, rgbaBuffer);
  // 释放缓冲区内存
//  free(rgbaBuffer);
  
    return sourceImage;
#elif defined(GPUPIXEL_IOS)
    auto path = Util::getResourcePath(name);
    UIImage* img =
        [UIImage imageNamed:[NSString stringWithUTF8String:path.c_str()]];
    return SourceImage::create(img);

#elif defined(GPUPIXEL_WIN)

#else
#endif
}

SourceImage::~SourceImage() {

}

SourceImage *SourceImage::setImage(int width, int height, const void *pixels) {
    this->setFramebuffer(0);
    if (!_framebuffer || (_framebuffer->getWidth() != width ||
                          _framebuffer->getHeight() != height)) {
        _framebuffer =
                GPUPixelContext::getInstance()->getFramebufferCache()->fetchFramebuffer(
                        width, height, true);
    }
    this->setFramebuffer(_framebuffer);
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, this->getFramebuffer()->getTexture()));
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, pixels));
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
    return this;
}

#if defined(GPUPIXEL_ANDROID)
std::shared_ptr<SourceImage> SourceImage::createImageForAndroid(std::string name) {
    auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
    // Todo(Jeayo) @see https://developer.android.com/ndk/guides/image-decoder?hl=zh-cn
    JavaVM *jvm = GetJVM();
    JNIEnv *env = GetEnv(jvm);

    // 定义类路径和方法签名
    const char *className = "com/pixpark/gpupixel/GPUPixelSourceImage";
    const char *methodName = "createBitmap";
    const char *methodSignature = "(Landroid/content/Context;Ljava/lang/String;)Landroid/graphics/Bitmap;";

    // GPUPixelSourceImage
    jclass jSourceImageClass = env->FindClass(className);
    if (jSourceImageClass == NULL) {
        return NULL;
    }

    // 找到createBitmap方法
    jmethodID createBitmapMethod = env->GetStaticMethodID(jSourceImageClass, methodName,
                                                          methodSignature);
    if (createBitmapMethod == NULL) {
        return NULL;
    }
    // get app context
    jobject jAppContext = NULL;
    const char *context_name = "android/app/ActivityThread";
    jclass activity_thread_clz = env->FindClass(context_name);
    if (activity_thread_clz != NULL) {
        jmethodID currentApplication = env->GetStaticMethodID(
                activity_thread_clz, "currentApplication", "()Landroid/app/Application;");
        if (currentApplication != NULL) {
            jAppContext = env->CallStaticObjectMethod(activity_thread_clz, currentApplication);
        } else {
            //todo
        }
        env->DeleteLocalRef(activity_thread_clz);
    } else {
        // todo
    }
    // 创建Java字符串作为参数
    jstring imgName = env->NewStringUTF(name.c_str());

    // 调用createBitmap方法
    jobject bitmap = env->CallStaticObjectMethod(jSourceImageClass,
                                                 createBitmapMethod,
                                                 jAppContext,
                                                 imgName);

    AndroidBitmapInfo info;
    GLvoid *pixels;
    AndroidBitmap_getInfo(env, bitmap, &info);
    if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        return nullptr;
    }
    AndroidBitmap_lockPixels(env, bitmap, &pixels);
    sourceImage->setImage(info.width, info.height, pixels);
    AndroidBitmap_unlockPixels(env, bitmap);

    // 释放资源
    env->DeleteLocalRef(imgName);
    env->DeleteLocalRef(jSourceImageClass);
    return sourceImage;
}

#endif
#if defined(GPUPIXEL_IOS)
std::shared_ptr<SourceImage> SourceImage::create(NSURL* imageUrl) {
  auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
  sourceImage->setImage(imageUrl);
  return sourceImage;
}

std::shared_ptr<SourceImage> SourceImage::create(NSData* imageData) {
  auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
  sourceImage->setImage(imageData);
  return sourceImage;
}

SourceImage* SourceImage::setImage(NSData* imageData) {
  UIImage* inputImage = [[UIImage alloc] initWithData:imageData];
  setImage(inputImage);
  return this;
}

std::shared_ptr<SourceImage> SourceImage::create(UIImage* image) {
  auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
  sourceImage->setImage(image);
  return sourceImage;
}

void SourceImage::setImage(UIImage* image) {
  UIImage* img = _adjustImageOrientation(image);
  setImage([img CGImage]);
}

std::shared_ptr<SourceImage> SourceImage::create(CGImageRef image) {
  auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
  sourceImage->setImage(image);
  return sourceImage;
}

SourceImage* SourceImage::setImage(NSURL* imageUrl) {
  NSData* imageData = [[NSData alloc] initWithContentsOfURL:imageUrl];
  setImage(imageData);
  return this;
}

SourceImage* SourceImage::setImage(CGImageRef image) {
  GLubyte* imageData = NULL;
  CFDataRef dataFromImageDataProvider =
      CGDataProviderCopyData(CGImageGetDataProvider(image));
  imageData = (GLubyte*)CFDataGetBytePtr(dataFromImageDataProvider);
  int width = (int)CGImageGetWidth(image);
  int height = (int)CGImageGetHeight(image);
  assert((width > 0 && height > 0) && "image can not be empty");

  setImage(width, height, imageData);

  CFRelease(dataFromImageDataProvider);

  return this;
}

UIImage* SourceImage::_adjustImageOrientation(UIImage* image) {
  if (image.imageOrientation == UIImageOrientationUp) {
    return image;
  }

  CGAffineTransform transform = CGAffineTransformIdentity;
  switch (image.imageOrientation) {
    case UIImageOrientationDown:
    case UIImageOrientationDownMirrored:
      transform = CGAffineTransformTranslate(transform, image.size.width,
                                             image.size.height);
      transform = CGAffineTransformRotate(transform, M_PI);
      break;
    case UIImageOrientationLeft:
    case UIImageOrientationLeftMirrored:
      transform = CGAffineTransformTranslate(transform, image.size.width, 0);
      transform = CGAffineTransformRotate(transform, M_PI_2);
      break;
    case UIImageOrientationRight:
    case UIImageOrientationRightMirrored:
      transform = CGAffineTransformTranslate(transform, 0, image.size.height);
      transform = CGAffineTransformRotate(transform, -M_PI_2);
      break;
    default:
      break;
  }

  switch (image.imageOrientation) {
    case UIImageOrientationUpMirrored:
    case UIImageOrientationDownMirrored:
      transform = CGAffineTransformTranslate(transform, image.size.width, 0);
      transform = CGAffineTransformScale(transform, -1, 1);
      break;
    case UIImageOrientationLeftMirrored:
    case UIImageOrientationRightMirrored:
      transform = CGAffineTransformTranslate(transform, image.size.height, 0);
      transform = CGAffineTransformScale(transform, -1, 1);
      break;
    default:
      break;
  }

  CGContextRef ctx = CGBitmapContextCreate(
      NULL, image.size.width, image.size.height,
      CGImageGetBitsPerComponent(image.CGImage), 0,
      CGImageGetColorSpace(image.CGImage), CGImageGetBitmapInfo(image.CGImage));
  CGContextConcatCTM(ctx, transform);
  switch (image.imageOrientation) {
    case UIImageOrientationLeft:
    case UIImageOrientationLeftMirrored:
    case UIImageOrientationRight:
    case UIImageOrientationRightMirrored:
      CGContextDrawImage(ctx,
                         CGRectMake(0, 0, image.size.height, image.size.width),
                         image.CGImage);
      break;
    default:
      CGContextDrawImage(ctx,
                         CGRectMake(0, 0, image.size.width, image.size.height),
                         image.CGImage);
      break;
  }

  CGImageRef cgImage = CGBitmapContextCreateImage(ctx);
  UIImage* newImage = [UIImage imageWithCGImage:cgImage];
  CGContextRelease(ctx);
  CGImageRelease(cgImage);
  return newImage;
}
#endif
