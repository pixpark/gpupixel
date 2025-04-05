LOCAL_PATH := $(call my-dir)

# 引入libyuv库
include $(CLEAR_VARS)
LOCAL_MODULE := libyuv_static
LOCAL_SRC_FILES := ../../../third_party/libyuv/lib/armeabi-v7a/libyuv_static.a
include $(PREBUILT_STATIC_LIBRARY)

# 主模块
include $(CLEAR_VARS)
LOCAL_MODULE := gpupixel
LOCAL_SRC_FILES := \
    jni_gpupixel.cc \
    jni_util.cc

LOCAL_C_INCLUDES := \
    $(LOCAL_PATH)/../../../include \
    $(LOCAL_PATH)/../../../src \
    $(LOCAL_PATH)/../../../third_party/libyuv/include

LOCAL_LDLIBS := -llog -lEGL -lGLESv2 -landroid -ljnigraphics
LOCAL_STATIC_LIBRARIES := libyuv_static
LOCAL_SHARED_LIBRARIES := gpupixel_static

include $(BUILD_SHARED_LIBRARY) 