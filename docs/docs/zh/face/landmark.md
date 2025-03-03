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
FaceDetector detector;

// 检测器将自动使用资源路径进行初始化
```

### 人脸检测和特征点提取
```cpp
// 注册回调函数以接收检测结果
detector.RegCallback([](std::vector<float> landmarks) {
    // 处理特征点数据
    // landmarks包含所有111个点的归一化坐标(x,y)
});

// 对图像数据进行检测
detector.Detect(
    imageData,      // 图像数据指针
    width,          // 图像宽度
    height,         // 图像高度
    format,         // 图像格式
    frameType       // 帧类型
);
```

### 支持的图像格式
SDK支持多种输入格式，包括：
- RGBA8888
- YUVI420

## 性能考虑
- SDK使用MNN框架针对移动端和嵌入式设备进行了优化
- 特征点坐标已归一化到[0,1]范围
- 通过回调机制提供检测结果，实现高效集成

## API参考

### FaceDetector类
```cpp
class FaceDetector {
    public:
        FaceDetector();
        ~FaceDetector();
        
        // 对图像数据进行人脸检测
        int Detect(const uint8_t* data,
                   int width,
                   int height,
                   GPUPIXEL_MODE_FMT fmt,
                   GPUPIXEL_FRAME_TYPE type);
        
        // 注册检测结果回调函数
        int RegCallback(FaceDetectorCallback callback);
};
```

### 回调函数
```cpp
typedef std::function<void(std::vector<float> landmarks)> FaceDetectorCallback;
```

### 检测结果
回调中的landmarks向量包含所有111个点的归一化坐标(x,y)，按顺序排列：
- 索引0-211：基础106个特征点（x,y对）
- 索引212-221：扩展的5个特征点（x,y对）