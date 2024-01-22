//
//  NebulaShaderType.h.h
//  zp-sdk-avmodule
//
//  Created by caozijun on 2023/2/27.
//

#ifndef NebulaShaderType_h_h
#define NebulaShaderType_h_h
#include <simd/simd.h>

// 存储数据的自定义结构，用于桥接 OC 和 Metal 代码（顶点）。
typedef struct {
  // 顶点坐标，4 维向量。
  vector_float4 position;
  // 纹理坐标。
  vector_float2 textureCoordinate;
} NebulaVertex;

// 存储数据的自定义结构，用于桥接 OC 和 Metal 代码（顶点）。
typedef struct {
  // YUV 矩阵。
  matrix_float3x3 matrix;
  // 是否为 full range。
  bool fullRange;
} NebulaConvertMatrix;

// 自定义枚举，用于桥接 OC 和 Metal 代码（顶点）。
// 顶点的桥接枚举值 KFVertexInputIndexVertices。
typedef enum NebulaVertexInputIndex {
  NebulaVertexInputIndexVertices = 0,
} KFVertexInputIndex;

// 自定义枚举，用于桥接 OC 和 Metal 代码（片元）。
// YUV 矩阵的桥接枚举值 NebulaFragmentInputIndexMatrix。
typedef enum NebulaFragmentBufferIndex {
  NebulaFragmentInputIndexMatrix = 0,
} NebulaMetalFragmentBufferIndex;

// 自定义枚举，用于桥接 OC 和 Metal 代码（片元）。
// YUV 数据的桥接枚举值
// NebulaFragmentTextureIndexTextureY、NebulaFragmentTextureIndexTextureUV。
typedef enum NebulaI420FragmentYUVTextureIndex {
  NebulaI420FragmentTextureIndexTextureY = 0,
  NebulaI420FragmentTextureIndexTextureU = 1,
  NebulaI420FragmentTextureIndexTextureV = 2,
} NebulaI420FragmentYUVTextureIndex;

typedef enum NebulaNV12FragmentTextureIndex {
  NebulaNV12FragmentTextureIndexTextureY = 0,
  NebulaNV12FragmentTextureIndexTextureUV = 1,
} NebulaNV12FragmentYUVTextureIndex;


// 自定义枚举，用于桥接 OC 和 Metal 代码（片元）。
// RGBA 数据的桥接枚举值 KFFragmentTextureIndexTextureRGB。
typedef enum NebulaFragmentRGBTextureIndex {
  NebulaFragmentTextureIndexTextureRGB = 0,
} NebulaFragmentRGBTextureIndex;

#endif /* NebulaShaderType_h_h */
