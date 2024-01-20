//
//  NebulaShaderType.metal
//  zp-sdk-avmodule
//
//  Created by caozijun on 2023/2/27.
//

#include "NebulaShaderType.h"
#include <metal_stdlib>
using namespace metal;
// 定义了一个类型为 RasterizerData 的结构体，里面有一个 float4 向量和 float2
// 向量。
typedef struct {
  // float4：4 维向量；
  // clipSpacePosition：参数名，表示顶点；
  // [[position]]：position
  // 是顶点修饰符，这是苹果内置的语法，不能改变，表示顶点信息。
  float4 clipSpacePosition [[position]];
  // float2：2 维向量；
  // textureCoordinate：参数名，这里表示纹理。
  float2 textureCoordinate;
} RasterizerData;

// 顶点函数通过一个自定义的结构体，返回对应的数据；顶点函数的输入参数也可以是自定义结构体。

// 顶点函数
// vertex：函数修饰符，表示顶点函数；
// RasterizerData：返回值类型；
// vertexShader：函数名；
// [[vertex_id]]：vertex_id 是顶点 id 修饰符，苹果内置的语法不可改变；
// [[buffer(YYImageVertexInputIndexVertexs)]]：buffer
// 是缓存数据修饰符，苹果内置的语法不可改变，YYImageVertexInputIndexVertexs
// 是索引； constant：是变量类型修饰符，表示存储在 device 区域。
vertex RasterizerData vertexShader(uint vertexID [[vertex_id]],
                                   constant NebulaVertex *vertexArray
                                   [[buffer(NebulaVertexInputIndexVertices)]]) {
  RasterizerData out;
  out.clipSpacePosition = vertexArray[vertexID].position;
  out.textureCoordinate = vertexArray[vertexID].textureCoordinate;

  return out;
}

// 片元函数
// fragment：函数修饰符，表示片元函数；
// float4：返回值类型，返回 RGBA；
// fragmentImageShader：函数名；
// RasterizerData：参数类型；
// input：变量名；
// [[stage_in]：stage_in
// 表示这个数据来自光栅化，光栅化是顶点处理之后的步骤，业务层无法修改。
// texture2d：类型表示纹理；
// textureY：表示 Y 通道；
// textureU：表示 U 通道；
// textureV：表示 V 通道；
// [[texture(index)]]：纹理修饰符；可以加索引：[[texture(0)]] 对应纹理
// 0，[[texture(1)]] 对应纹理 1；
// NebulaI420FragmentTextureIndexTextureY、NebulaI420FragmentTextureIndexTextureU、NebulaI420FragmentTextureIndexTextureV：表示纹理索引。
fragment float4 yuvSamplingShader(
  RasterizerData input [[stage_in]],
  texture2d<float> textureY [[texture(NebulaI420FragmentTextureIndexTextureY)]],
  texture2d<float> textureU [[texture(NebulaI420FragmentTextureIndexTextureU)]],
  texture2d<float> textureV [[texture(NebulaI420FragmentTextureIndexTextureV)]],
  constant NebulaConvertMatrix *convertMatrix
  [[buffer(NebulaFragmentInputIndexMatrix)]]) {
  constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);
  float3 yuv;

  if (convertMatrix->fullRange) { // full range.
    yuv.x = textureY.sample(textureSampler, input.textureCoordinate).r;
  } else { // video range.
    yuv.x = textureY.sample(textureSampler, input.textureCoordinate).r -
            (16.0 / 255.0);
  }
  yuv.y = textureU.sample(textureSampler, input.textureCoordinate).r - 0.5;
  yuv.z = textureV.sample(textureSampler, input.textureCoordinate).r - 0.5;
  float3 rgb = convertMatrix->matrix * yuv;

  return float4(rgb, 1.0);
}


// 片元函数
// fragment：函数修饰符，表示片元函数；
// float4：返回值类型，返回 RGBA；
// fragmentImageShader：函数名；
// RasterizerData：参数类型；
// input：变量名；
// [[stage_in]：stage_in
// 表示这个数据来自光栅化，光栅化是顶点处理之后的步骤，业务层无法修改。
// texture2d：类型表示纹理；
// textureY：表示 Y 通道；
// textureUV：表示 UV 通道；
// [[texture(index)]]：纹理修饰符；可以加索引：[[texture(0)]] 对应纹理
// 0，[[texture(1)]] 对应纹理 1；
// NebulaI420FragmentTextureIndexTextureV、NebulaNV12FragmentTextureIndexTextureUV：表示纹理索引。

fragment float4 nv12SamplingShader(
  RasterizerData input [[stage_in]],
  texture2d<float> textureY [[texture(NebulaNV12FragmentTextureIndexTextureY)]],
  texture2d<float> textureUV [[texture(NebulaNV12FragmentTextureIndexTextureUV)]],
  constant NebulaConvertMatrix *convertMatrix
  [[buffer(NebulaFragmentInputIndexMatrix)]]) {
  constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);
  float3 yuv;

  if (convertMatrix->fullRange) { // full range.
    yuv.x = textureY.sample(textureSampler, input.textureCoordinate).r;
  } else { // video range.
    yuv.x = textureY.sample(textureSampler, input.textureCoordinate).r -
            (16.0 / 255.0);
  }
  yuv.yz = textureUV.sample(textureSampler, input.textureCoordinate).rg - 0.5;
  float3 rgb = convertMatrix->matrix * yuv;
      
  return float4(rgb, 1.0);
}

// 片元函数
// fragment：函数修饰符，表示片元函数；
// float4：返回值类型，返回 RGBA；
// fragmentImageShader：函数名；
// RasterizerData：参数类型；
// input：变量名；
// [[stage_in]：stage_in
// 表示这个数据来自光栅化，光栅化是顶点处理之后的步骤，业务层无法修改。
// texture2d：类型表示纹理；
// colorTexture：代表 RGBA 数据；
// [[texture(index)]]：纹理修饰符；可以加索引：[[texture(0)]] 对应纹理
// 0，[[texture(1)]] 对应纹理 1；
// KFFragmentTextureIndexTextureRGB：表示纹理索引。
fragment float4 rgbSamplingShader(
    RasterizerData input [[stage_in]], texture2d<half> colorTexture
    [[texture(NebulaFragmentTextureIndexTextureRGB)]]) {
  constexpr sampler textureSampler(mag_filter::linear, min_filter::linear);

  half4 colorSample =
      colorTexture.sample(textureSampler, input.textureCoordinate);

  return float4(colorSample);
}
