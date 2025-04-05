#include <android/bitmap.h>
#include <jni.h>
#include <list>
#include <string>

#include "gpupixel_context.h"
#include "jni_helpers.h"
#include "source_image.h"

using namespace gpupixel;

// Maintain all created SourceImage objects
static std::list<std::shared_ptr<SourceImage>> source_image_list_;

// Create new SourceImage instance - from file path
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeCreateFromFile(
    JNIEnv* env,
    jclass clazz,
    jstring file_path) {
  const char* path = env->GetStringUTFChars(file_path, NULL);
  auto source_image = SourceImage::Create(path);
  env->ReleaseStringUTFChars(file_path, path);

  if (!source_image) {
    return 0;
  }
  source_image_list_.push_back(source_image);
  return (jlong)(source_image.get());
}

// Create new SourceImage instance - from buffer
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeCreateFromBuffer(
    JNIEnv* env,
    jclass clazz,
    jint width,
    jint height,
    jint channel_count,
    jbyteArray data) {
  jbyte* bytes = env->GetByteArrayElements(data, NULL);

  auto source_image = SourceImage::CreateFromBuffer(
      width, height, channel_count, (const unsigned char*)bytes);

  env->ReleaseByteArrayElements(data, bytes, 0);

  if (!source_image) {
    return 0;
  }
  source_image_list_.push_back(source_image);
  return (jlong)(source_image.get());
}

// Create new SourceImage instance - from Bitmap
extern "C" JNIEXPORT jlong JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeCreateFromBitmap(
    JNIEnv* env,
    jclass clazz,
    jobject bitmap) {
  AndroidBitmapInfo info;
  if (AndroidBitmap_getInfo(env, bitmap, &info) < 0) {
    return 0;
  }

  void* pixels;
  if (AndroidBitmap_lockPixels(env, bitmap, &pixels) < 0) {
    return 0;
  }

  auto source_image = SourceImage::CreateFromBuffer(
      info.width, info.height, 4, (const unsigned char*)pixels);

  AndroidBitmap_unlockPixels(env, bitmap);

  if (!source_image) {
    return 0;
  }
  source_image_list_.push_back(source_image);
  return (jlong)(source_image.get());
}

// Destroy SourceImage instance
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeDestroy(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong native_obj) {
  for (auto si : source_image_list_) {
    if ((jlong)si.get() == native_obj) {
      source_image_list_.remove(si);
      break;
    }
  }
}

// Release SourceImage framebuffer
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeFinalize(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  ((SourceImage*)native_obj)->ReleaseFramebuffer(false);
}

// Get image width
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeGetWidth(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  return ((SourceImage*)native_obj)->GetWidth();
}

// Get image height
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeGetHeight(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  return ((SourceImage*)native_obj)->GetHeight();
}

// Get RGBA image data
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeGetRgbaImageBuffer(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  const unsigned char* buffer =
      ((SourceImage*)native_obj)->GetRgbaImageBuffer();
  if (!buffer) {
    return NULL;
  }

  int width = ((SourceImage*)native_obj)->GetWidth();
  int height = ((SourceImage*)native_obj)->GetHeight();
  int size = width * height * 4;  // RGBA = 4 bytes per pixel

  jbyteArray result = env->NewByteArray(size);
  env->SetByteArrayRegion(result, 0, size, (jbyte*)buffer);

  return result;
}

// Execute rendering
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeRender(JNIEnv* env,
                                                           jclass clazz,
                                                           jlong native_obj) {
  ((SourceImage*)native_obj)->Render();
}
