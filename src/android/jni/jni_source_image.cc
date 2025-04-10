#include <android/bitmap.h>
#include <jni.h>
#include <list>
#include <string>

#include "android/jni/jni_helpers.h"
#include "core/gpupixel_context.h"
#include "gpupixel/source/source_image.h"

using namespace gpupixel;

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

  // Create shared_ptr on heap
  auto* ptr = new std::shared_ptr<SourceImage>(source_image);
  return reinterpret_cast<jlong>(ptr);
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

  // Create shared_ptr on heap
  auto* ptr = new std::shared_ptr<SourceImage>(source_image);
  return reinterpret_cast<jlong>(ptr);
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

  // Create shared_ptr on heap
  auto* ptr = new std::shared_ptr<SourceImage>(source_image);
  return reinterpret_cast<jlong>(ptr);
}

// Destroy SourceImage instance
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeDestroy(JNIEnv* env,
                                                            jclass clazz,
                                                            jlong native_obj) {
  // Free the heap-allocated shared_ptr
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceImage>*>(native_obj);
  delete ptr;
}

// Release SourceImage framebuffer
extern "C" JNIEXPORT void JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeFinalize(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  // Get shared_ptr and access the object
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceImage>*>(native_obj);
  if (ptr && *ptr) {
    (*ptr)->ReleaseFramebuffer(false);
  }
}

// Get image width
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeGetWidth(JNIEnv* env,
                                                             jclass clazz,
                                                             jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceImage>*>(native_obj);
  return ptr && *ptr ? (*ptr)->GetWidth() : 0;
}

// Get image height
extern "C" JNIEXPORT jint JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeGetHeight(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceImage>*>(native_obj);
  return ptr && *ptr ? (*ptr)->GetHeight() : 0;
}

// Get RGBA image data
extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_pixpark_gpupixel_GPUPixelSourceImage_nativeGetRgbaImageBuffer(
    JNIEnv* env,
    jclass clazz,
    jlong native_obj) {
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceImage>*>(native_obj);
  if (!ptr || !*ptr) {
    return NULL;
  }

  const unsigned char* buffer = (*ptr)->GetRgbaImageBuffer();
  if (!buffer) {
    return NULL;
  }

  int width = (*ptr)->GetWidth();
  int height = (*ptr)->GetHeight();
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
  auto* ptr = reinterpret_cast<std::shared_ptr<SourceImage>*>(native_obj);
  if (ptr && *ptr) {
    (*ptr)->Render();
  }
}
