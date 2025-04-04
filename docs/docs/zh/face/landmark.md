# 人脸关键点

## 概述
GPUPixel使用Mars Face进行Landmark检测，是一个基于MNN（Mobile Neural Network）框架的高性能人脸检测和面部特征点检测解决方案。它提供精确的人脸检测功能，可返回106个基础面部特征点，并扩展到111个特征点以实现更详细的面部特征分析。

## 功能特点
- 基于MNN框架的高性能人脸检测
- 106个基础面部特征点检测
- 扩展至111个面部特征点
- 跨平台支持
- 简单易用的API接口

## 基础面部特征点
SDK检测的106个基础面部特征点覆盖了以下关键面部特征：
- 眼睛轮廓
- 眉毛
- 鼻子
- 嘴部轮廓
- 面部轮廓

## 扩展特征点
基于106个基础特征点，SDK通过特定面部特征点计算得出5个额外的特征点（107-111）：

- 点106：特征点102和98的中点
- 点107：特征点35和65的中点
- 点108：特征点70和40的中点
- 点109：特征点5和80的中点
- 点110：特征点81和27的中点

这些额外的特征点为高级应用提供了更详细的面部特征信息。

## 点序
![](../../image/landmark-dot.jpg)

## 集成指南

### 初始化
```cpp
#include "face_detector.h"

// 创建人脸检测器实例
std::shared_ptr<FaceDetector> faceDetector = std::make_shared<FaceDetector>();
```

### 人脸检测和特征点提取
```cpp
// 直接对图像数据进行检测并获取特征点
std::vector<float> landmarks = faceDetector->Detect(
    buffer,                     // 图像数据指针 (const unsigned char*)
    width,                      // 图像宽度 (int)
    height,                     // 图像高度 (int)
    GPUPIXEL_MODE_FMT_PICTURE,  // 图像格式 (GPUPIXEL_MODE_FMT)
    GPUPIXEL_FRAME_TYPE_RGBA    // 帧类型 (GPUPIXEL_FRAME_TYPE)
);

// 检查是否检测到特征点
if (!landmarks.empty()) {
    // 将特征点应用到滤镜
    lipstickFilter->SetFaceLandmarks(landmarks);
    blusherFilter->SetFaceLandmarks(landmarks);
    reshapeFilter->SetFaceLandmarks(landmarks);
}
```

### 支持的图像格式
检测器支持多种输入格式：

**格式参数 (GPUPIXEL_MODE_FMT)**:
- `GPUPIXEL_MODE_FMT_VIDEO`: 视频源格式
- `GPUPIXEL_MODE_FMT_PICTURE`: 静态图像格式

**帧类型参数 (GPUPIXEL_FRAME_TYPE)**:
- `GPUPIXEL_FRAME_TYPE_YUVI420`: YUV I420格式
- `GPUPIXEL_FRAME_TYPE_RGBA`: RGBA格式
- `GPUPIXEL_FRAME_TYPE_BGRA`: BGRA格式

## 性能考虑
- 检测器针对实时视频和静态图像处理进行了优化
- 特征点坐标已归一化到[0,1]范围
- 检测结果直接作为float向量返回

## API参考

### FaceDetector类
```cpp
class FaceDetector {
public:
    FaceDetector();
    ~FaceDetector();
    
    // 对图像数据进行人脸检测并返回特征点
    std::vector<float> Detect(
        const uint8_t* data,         // 图像数据指针
        int width,                   // 图像宽度
        int height,                  // 图像高度
        GPUPIXEL_MODE_FMT fmt,       // 图像格式
        GPUPIXEL_FRAME_TYPE type     // 帧类型
    );
};
```

### 特征点结果格式
返回的landmarks向量包含所有检测到的点的归一化坐标(x,y)，按顺序排列：
- 对于每个特征点，两个连续的float值表示其(x,y)坐标
- 坐标已归一化到图像尺寸范围（0.0到1.0）
- 如果未检测到人脸，则返回空向量