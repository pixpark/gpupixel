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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "face_detector.h"
USING_NS_GPUPIXEL

std::shared_ptr<SourceImage> SourceImage::create_from_memory(int width,
                                                 int height,
                                                 int channel_count,
                                                 const unsigned char *pixels) {
    auto sourceImage = std::shared_ptr<SourceImage>(new SourceImage());
    sourceImage->init(width, height, channel_count, pixels);
    return sourceImage;
}

std::shared_ptr<SourceImage> SourceImage::create(const std::string name) {
#if defined(GPUPIXEL_ANDROID)
    auto sourceImage = createImageForAndroid(name);
    return sourceImage;
#else
  int width, height, channel_count;
  unsigned char *data = stbi_load(name.c_str(), &width, &height, &channel_count, 0);
//   todo(logo info)
  if(data == nullptr) {
    Util::Log("SourceImage", "SourceImage: input data in null! file name: %s", name.c_str());
    return nullptr;
  }
  auto image = SourceImage::create_from_memory(width, height, channel_count, data);
  stbi_image_free(data);
  return image;
#endif
}

void SourceImage::init(int width, int height, int channel_count, const unsigned char* pixels) {
    this->setFramebuffer(0);
    if (!_framebuffer || (_framebuffer->getWidth() != width ||
                            _framebuffer->getHeight() != height)) {
        _framebuffer =
                GPUPixelContext::getInstance()->getFramebufferCache()->fetchFramebuffer(
                        width, height, true);
    }
    this->setFramebuffer(_framebuffer);
    CHECK_GL(glBindTexture(GL_TEXTURE_2D, this->getFramebuffer()->getTexture()));
  
  image_channel_count_ = channel_count;
  if(channel_count == 3) {
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                          GL_UNSIGNED_BYTE, pixels));
    image_bytes.assign(pixels, pixels + width * height *3);
   
  } else if(channel_count == 4) {
    CHECK_GL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                          GL_UNSIGNED_BYTE, pixels));
    image_bytes.assign(pixels, pixels + width * height *4);
  }
  CHECK_GL(glBindTexture(GL_TEXTURE_2D, 0));
}

void SourceImage::Render() {
  GPUPIXEL_FRAME_TYPE type;
  if(_face_detector) {
    if(image_channel_count_ == 3) {
      type = GPUPIXEL_FRAME_TYPE_RGB888;
      // todo vnn not support, only support png picture.
      std::terminate();
    } else if(image_channel_count_ == 4) {
      type = GPUPIXEL_FRAME_TYPE_RGBA8888;
    }

    _face_detector->Detect(image_bytes.data(), _framebuffer->getWidth(), _framebuffer->getHeight(), type);
  }
  
  Source::proceed();

}

#if defined(GPUPIXEL_ANDROID)
std::shared_ptr<SourceImage> SourceImage::createImageForAndroid(std::string name) {
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
 
    auto sourceImage = SourceImage::create_from_memory(info.width, info.height, 4, (const unsigned char *)pixels);
    AndroidBitmap_unlockPixels(env, bitmap);

    // 释放资源
    env->DeleteLocalRef(imgName);
    env->DeleteLocalRef(jSourceImageClass);
    return sourceImage;
}
#endif
 
