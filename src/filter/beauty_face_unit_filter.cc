/*
 * GPUPixel
 *
 * Created by PixPark on 2021/6/24.
 * Copyright © 2021 PixPark. All rights reserved.
 */

#include "gpupixel/filter/beauty_face_unit_filter.h"
#include "core/gpupixel_context.h"
#include "gpupixel/source/source_image.h"
#include "utils/util.h"

namespace gpupixel {
const std::string kGPUImageBaseBeautyFaceVertexShaderString = R"(
    attribute vec3 position; attribute vec2 inputTextureCoordinate;

    varying vec2 textureCoordinate;
    varying vec4 textureShift_1;
    varying vec4 textureShift_2;
    varying vec4 textureShift_3;
    varying vec4 textureShift_4;

    uniform float widthOffset;
    uniform float heightOffset;
    void main(void) {
      gl_Position = vec4(position, 1.0);
      textureCoordinate = inputTextureCoordinate;
      textureShift_1 = vec4(inputTextureCoordinate + vec2(-widthOffset, 0.0),
                            inputTextureCoordinate + vec2(widthOffset, 0.0));
      textureShift_2 = vec4(inputTextureCoordinate + vec2(0.0, -heightOffset),
                            inputTextureCoordinate + vec2(0.0, heightOffset));
      textureShift_3 =
          vec4(inputTextureCoordinate + vec2(widthOffset, heightOffset),
               inputTextureCoordinate + vec2(-widthOffset, -heightOffset));
      textureShift_4 =
          vec4(inputTextureCoordinate + vec2(-widthOffset, heightOffset),
               inputTextureCoordinate + vec2(widthOffset, -heightOffset));
    })";
#if defined(GPUPIXEL_GLES_SHADER)
const std::string kGPUImageBaseBeautyFaceFragmentShaderString = R"(
    precision highp float; 
    varying highp vec2 textureCoordinate;
    varying highp vec4 textureShift_1;
    varying highp vec4 textureShift_2;
    varying highp vec4 textureShift_3;
    varying highp vec4 textureShift_4;

    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;
    uniform sampler2D inputImageTexture3;
    uniform sampler2D lookUpGray;
    uniform sampler2D lookUpOrigin;
    uniform sampler2D lookUpSkin;
    uniform sampler2D lookUpCustom;

    uniform highp float sharpen;
    uniform highp float blurAlpha;
    uniform highp float whiten;
    uniform highp float warmth;
    uniform highp float ruddy;

    const float levelRangeInv = 1.02657;
    const float levelBlack = 0.0258820;
    const float alpha = 0.7;

    void main() {
      vec4 iColor = texture2D(inputImageTexture, textureCoordinate);
      vec4 meanColor = texture2D(inputImageTexture2, textureCoordinate);
      vec4 varColor = texture2D(inputImageTexture3, textureCoordinate);

      vec3 color = iColor.rgb;
      if (blurAlpha >= 0.0) {
        float theta = 0.1;
        float p =
            clamp((min(iColor.r, meanColor.r - 0.1) - 0.2) * 4.0, 0.0, 1.0);
        float meanVar = (varColor.r + varColor.g + varColor.b) / 3.0;
        float kMin;
        highp vec3 resultColor;
        kMin = (1.0 - meanVar / (meanVar + theta)) * p * blurAlpha;
        kMin = clamp(kMin, 0.0, 1.0);
        resultColor = mix(iColor.rgb, meanColor.rgb, kMin);

        vec3 sum = 0.25 * iColor.rgb;
        sum += 0.125 * texture2D(inputImageTexture, textureShift_1.xy).rgb;
        sum += 0.125 * texture2D(inputImageTexture, textureShift_1.zw).rgb;
        sum += 0.125 * texture2D(inputImageTexture, textureShift_2.xy).rgb;
        sum += 0.125 * texture2D(inputImageTexture, textureShift_2.zw).rgb;
        sum += 0.0625 * texture2D(inputImageTexture, textureShift_3.xy).rgb;
        sum += 0.0625 * texture2D(inputImageTexture, textureShift_3.zw).rgb;
        sum += 0.0625 * texture2D(inputImageTexture, textureShift_4.xy).rgb;
        sum += 0.0625 * texture2D(inputImageTexture, textureShift_4.zw).rgb;

        vec3 hPass = iColor.rgb - sum;
        color = resultColor + sharpen * hPass * 2.0;
      }

      if (whiten > 0.0) {
        vec3 colorEPM = color;
        // 让级别调整受到 whiten 参数控制
        vec3 adjustedColor = clamp((colorEPM - vec3(levelBlack)) * levelRangeInv, 0.0, 1.0);
        color = mix(colorEPM, adjustedColor, whiten);
        
        vec3 texel = vec3(texture2D(lookUpGray, vec2(color.r, 0.5)).r,
                          texture2D(lookUpGray, vec2(color.g, 0.5)).g,
                          texture2D(lookUpGray, vec2(color.b, 0.5)).b);
        // 让灰度混合受到 whiten 参数控制
        texel = mix(color, texel, 0.5 * whiten);
        // 让最终混合受到 whiten 参数控制
        texel = mix(colorEPM, texel, alpha * whiten);

        texel = clamp(texel, 0., 1.);
        float blueColor = texel.b * 15.0;
        vec2 quad1;
        quad1.y = floor(floor(blueColor) * 0.25);
        quad1.x = floor(blueColor) - (quad1.y * 4.0);
        vec2 quad2;
        quad2.y = floor(ceil(blueColor) * 0.25);
        quad2.x = ceil(blueColor) - (quad2.y * 4.0);
        vec2 texPos2 = texel.rg * 0.234375 + 0.0078125;
        vec2 texPos1 = quad1 * 0.25 + texPos2;
        texPos2 = quad2 * 0.25 + texPos2;
        vec3 newColor1Origin = texture2D(lookUpOrigin, texPos1).rgb;
        vec3 newColor2Origin = texture2D(lookUpOrigin, texPos2).rgb;
        vec3 colorOrigin =
            mix(newColor1Origin, newColor2Origin, fract(blueColor));
        // 让原始查找表混合受到 whiten 参数控制
        texel = mix(color, colorOrigin, alpha * whiten);

        texel = clamp(texel, 0., 1.);
        blueColor = texel.b * 15.0;
        quad1.y = floor(floor(blueColor) * 0.25);
        quad1.x = floor(blueColor) - (quad1.y * 4.0);
        quad2.y = floor(ceil(blueColor) * 0.25);
        quad2.x = ceil(blueColor) - (quad2.y * 4.0);
        texPos2 = texel.rg * 0.234375 + 0.0078125;
        texPos1 = quad1 * 0.25 + texPos2;
        texPos2 = quad2 * 0.25 + texPos2;
        vec3 newColor1 = texture2D(lookUpSkin, texPos1).rgb;
        vec3 newColor2 = texture2D(lookUpSkin, texPos2).rgb;
        vec3 skinColor = mix(newColor1.rgb, newColor2.rgb, fract(blueColor));
        // 让皮肤查找表效果受到 whiten 参数控制
        color = mix(color, skinColor, whiten);
        color = clamp(color, 0., 1.);

        highp float blueColor_custom = color.b * 63.0;
        highp vec2 quad1_custom;
        quad1_custom.y = floor(floor(blueColor_custom) / 8.0);
        quad1_custom.x = floor(blueColor_custom) - (quad1_custom.y * 8.0);
        highp vec2 quad2_custom;
        quad2_custom.y = floor(ceil(blueColor_custom) / 8.0);
        quad2_custom.x = ceil(blueColor_custom) - (quad2_custom.y * 8.0);
        highp vec2 texPos1_custom;
        texPos1_custom.x = (quad1_custom.x * 1.0 / 8.0) + 0.5 / 512.0 +
                           ((1.0 / 8.0 - 1.0 / 512.0) * color.r);
        texPos1_custom.y = (quad1_custom.y * 1.0 / 8.0) + 0.5 / 512.0 +
                           ((1.0 / 8.0 - 1.0 / 512.0) * color.g);
        highp vec2 texPos2_custom;
        texPos2_custom.x = (quad2_custom.x * 1.0 / 8.0) + 0.5 / 512.0 +
                           ((1.0 / 8.0 - 1.0 / 512.0) * color.r);
        texPos2_custom.y = (quad2_custom.y * 1.0 / 8.0) + 0.5 / 512.0 +
                           ((1.0 / 8.0 - 1.0 / 512.0) * color.g);
        newColor1 = texture2D(lookUpCustom, texPos1_custom).rgb;
        newColor2 = texture2D(lookUpCustom, texPos2_custom).rgb;
        vec3 color_custom =
            mix(newColor1, newColor2, fract(blueColor_custom));
        color = mix(color, color_custom, whiten);
        
        // 应用色温调整（暖白/冷白效果）
        if (abs(warmth) > 0.001) {
          vec3 warmColor = color;
          if (warmth > 0.0) {
            // 暖白：增加红色和绿色，减少蓝色
            warmColor.r = color.r + warmth * 0.1;
            warmColor.g = color.g + warmth * 0.05;
            warmColor.b = color.b - warmth * 0.05;
          } else {
            // 冷白：增加蓝色，减少红色和绿色
            warmColor.r = color.r + warmth * 0.05;
            warmColor.g = color.g + warmth * 0.03;
            warmColor.b = color.b - warmth * 0.08;
          }
          color = clamp(warmColor, 0.0, 1.0);
        }
        
        // 应用红润效果
        if (ruddy > 0.001) {
          vec3 ruddyColor = color;
          // 增加红色通道，营造红润效果
          ruddyColor.r = color.r + ruddy * 0.15;
          // 适当增加绿色通道，让红润更自然
          ruddyColor.g = color.g + ruddy * 0.08;
          // 稍微减少蓝色通道，增强暖红色调
          ruddyColor.b = color.b - ruddy * 0.03;
          color = clamp(ruddyColor, 0.0, 1.0);
        }
      }

      gl_FragColor = vec4(color, 1.0);
    })";
#elif defined(GPUPIXEL_GL_SHADER)
const std::string kGPUImageBaseBeautyFaceFragmentShaderString = R"(
    float; varying vec2 textureCoordinate; varying vec4 textureShift_1;
    varying vec4 textureShift_2;
    varying vec4 textureShift_3;
    varying vec4 textureShift_4;

    uniform sampler2D inputImageTexture;
    uniform sampler2D inputImageTexture2;
    uniform sampler2D inputImageTexture3;
    uniform sampler2D lookUpGray;
    uniform sampler2D lookUpOrigin;
    uniform sampler2D lookUpSkin;
    uniform sampler2D lookUpCustom;

    uniform float sharpen;
    uniform float blurAlpha;
    uniform float whiten;
    uniform float warmth;
    uniform float ruddy;

    const float levelRangeInv = 1.02657;
    const float levelBlack = 0.0258820;
    const float alpha = 0.7;

    void main() {
      vec4 iColor = texture2D(inputImageTexture, textureCoordinate);
      vec4 meanColor = texture2D(inputImageTexture2, textureCoordinate);
      vec4 varColor = texture2D(inputImageTexture3, textureCoordinate);

  
      vec3 color = iColor.rgb;
      if (blurAlpha > 0.0) {
        float theta = 0.1;
        float p =
            clamp((min(iColor.r, meanColor.r - 0.1) - 0.2) * 4.0, 0.0, 1.0);
        float meanVar = (varColor.r + varColor.g + varColor.b) / 3.0;
        float kMin;
        vec3 resultColor;
        kMin = (1.0 - meanVar / (meanVar + theta)) * p * blurAlpha;
        kMin = clamp(kMin, 0.0, 1.0);
        resultColor = mix(iColor.rgb, meanColor.rgb, kMin);

        vec3 sum = 0.25 * iColor.rgb;
        sum += 0.125 * texture2D(inputImageTexture, textureShift_1.xy).rgb;
        sum += 0.125 * texture2D(inputImageTexture, textureShift_1.zw).rgb;
        sum += 0.125 * texture2D(inputImageTexture, textureShift_2.xy).rgb;
        sum += 0.125 * texture2D(inputImageTexture, textureShift_2.zw).rgb;
        sum += 0.0625 * texture2D(inputImageTexture, textureShift_3.xy).rgb;
        sum += 0.0625 * texture2D(inputImageTexture, textureShift_3.zw).rgb;
        sum += 0.0625 * texture2D(inputImageTexture, textureShift_4.xy).rgb;
        sum += 0.0625 * texture2D(inputImageTexture, textureShift_4.zw).rgb;

        vec3 hPass = iColor.rgb - sum;
        color = resultColor + sharpen * hPass * 2.0;
      }

      // whiten
      if (whiten > 0.0) {
        vec3 colorEPM = color;
        // 让级别调整受到 whiten 参数控制
        vec3 adjustedColor = clamp((colorEPM - vec3(levelBlack)) * levelRangeInv, 0.0, 1.0);
        color = mix(colorEPM, adjustedColor, whiten);
        
        vec3 texel = vec3(texture2D(lookUpGray, vec2(color.r, 0.5)).r,
                          texture2D(lookUpGray, vec2(color.g, 0.5)).g,
                          texture2D(lookUpGray, vec2(color.b, 0.5)).b);
        // 让灰度混合受到 whiten 参数控制
        texel = mix(color, texel, 0.5 * whiten);
        // 让最终混合受到 whiten 参数控制
        texel = mix(colorEPM, texel, alpha * whiten);

        texel = clamp(texel, 0., 1.);
        float blueColor = texel.b * 15.0;
        vec2 quad1;
        quad1.y = floor(floor(blueColor) * 0.25);
        quad1.x = floor(blueColor) - (quad1.y * 4.0);
        vec2 quad2;
        quad2.y = floor(ceil(blueColor) * 0.25);
        quad2.x = ceil(blueColor) - (quad2.y * 4.0);
        vec2 texPos2 = texel.rg * 0.234375 + 0.0078125;
        vec2 texPos1 = quad1 * 0.25 + texPos2;
        texPos2 = quad2 * 0.25 + texPos2;
        vec3 newColor1Origin = texture2D(lookUpOrigin, texPos1).rgb;
        vec3 newColor2Origin = texture2D(lookUpOrigin, texPos2).rgb;
        vec3 colorOrigin =
            mix(newColor1Origin, newColor2Origin, fract(blueColor));
        // 让原始查找表混合受到 whiten 参数控制
        texel = mix(color, colorOrigin, alpha * whiten);

        texel = clamp(texel, 0., 1.);
        blueColor = texel.b * 15.0;
        quad1.y = floor(floor(blueColor) * 0.25);
        quad1.x = floor(blueColor) - (quad1.y * 4.0);
        quad2.y = floor(ceil(blueColor) * 0.25);
        quad2.x = ceil(blueColor) - (quad2.y * 4.0);
        texPos2 = texel.rg * 0.234375 + 0.0078125;
        texPos1 = quad1 * 0.25 + texPos2;
        texPos2 = quad2 * 0.25 + texPos2;
        vec3 newColor1 = texture2D(lookUpSkin, texPos1).rgb;
        vec3 newColor2 = texture2D(lookUpSkin, texPos2).rgb;
        vec3 skinColor = mix(newColor1.rgb, newColor2.rgb, fract(blueColor));
        // 让皮肤查找表效果受到 whiten 参数控制
        color = mix(color, skinColor, whiten);
        color = clamp(color, 0., 1.);

        float blueColor_custom = color.b * 63.0;
        vec2 quad1_custom;
        quad1_custom.y = floor(floor(blueColor_custom) / 8.0);
        quad1_custom.x = floor(blueColor_custom) - (quad1_custom.y * 8.0);
        vec2 quad2_custom;
        quad2_custom.y = floor(ceil(blueColor_custom) / 8.0);
        quad2_custom.x = ceil(blueColor_custom) - (quad2_custom.y * 8.0);
        vec2 texPos1_custom;
        texPos1_custom.x = (quad1_custom.x * 1.0 / 8.0) + 0.5 / 512.0 +
                           ((1.0 / 8.0 - 1.0 / 512.0) * color.r);
        texPos1_custom.y = (quad1_custom.y * 1.0 / 8.0) + 0.5 / 512.0 +
                           ((1.0 / 8.0 - 1.0 / 512.0) * color.g);
        vec2 texPos2_custom;
        texPos2_custom.x = (quad2_custom.x * 1.0 / 8.0) + 0.5 / 512.0 +
                           ((1.0 / 8.0 - 1.0 / 512.0) * color.r);
        texPos2_custom.y = (quad2_custom.y * 1.0 / 8.0) + 0.5 / 512.0 +
                           ((1.0 / 8.0 - 1.0 / 512.0) * color.g);
        newColor1 = texture2D(lookUpCustom, texPos1_custom).rgb;
        newColor2 = texture2D(lookUpCustom, texPos2_custom).rgb;
        vec3 color_custom =
            mix(newColor1, newColor2, fract(blueColor_custom));
        color = mix(color, color_custom, whiten);
        
        // 应用色温调整（暖白/冷白效果）
        if (abs(warmth) > 0.001) {
          vec3 warmColor = color;
          if (warmth > 0.0) {
            // 暖白：增加红色和绿色，减少蓝色
            warmColor.r = color.r + warmth * 0.1;
            warmColor.g = color.g + warmth * 0.05;
            warmColor.b = color.b - warmth * 0.05;
          } else {
            // 冷白：增加蓝色，减少红色和绿色
            warmColor.r = color.r + warmth * 0.05;
            warmColor.g = color.g + warmth * 0.03;
            warmColor.b = color.b - warmth * 0.08;
          }
          color = clamp(warmColor, 0.0, 1.0);
        }
        
        // 应用红润效果
        if (ruddy > 0.001) {
          vec3 ruddyColor = color;
          // 增加红色通道，营造红润效果
          ruddyColor.r = color.r + ruddy * 0.15;
          // 适当增加绿色通道，让红润更自然
          ruddyColor.g = color.g + ruddy * 0.08;
          // 稍微减少蓝色通道，增强暖红色调
          ruddyColor.b = color.b - ruddy * 0.03;
          color = clamp(ruddyColor, 0.0, 1.0);
        }
      }
      
      gl_FragColor = vec4(color, 1.0);
      
    })";
#endif

BeautyFaceUnitFilter::BeautyFaceUnitFilter() {}

BeautyFaceUnitFilter::~BeautyFaceUnitFilter() {}

std::shared_ptr<BeautyFaceUnitFilter> BeautyFaceUnitFilter::Create() {
  auto ret = std::shared_ptr<BeautyFaceUnitFilter>(new BeautyFaceUnitFilter());
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext([&] {
    if (ret && !ret->Init()) {
      ret.reset();
    }
  });
  return ret;
}

bool BeautyFaceUnitFilter::Init() {
  if (!Filter::InitWithShaderString(kGPUImageBaseBeautyFaceVertexShaderString,
                                    kGPUImageBaseBeautyFaceFragmentShaderString,
                                    3)) {
    return false;
  }

  // 不再在这里直接创建 SourceImage，而是等待外部通过 SetLookupImages 传入
  return true;
}

void BeautyFaceUnitFilter::SetLookupImages(
    const std::vector<std::shared_ptr<SourceImage>>& lookup_images) {
  if (lookup_images.size() >= 4) {
    gray_image_ = lookup_images[0];      // gray
    original_image_ = lookup_images[1];  // original
    skin_image_ = lookup_images[2];      // skin
    custom_image_ = lookup_images[3];    // custom
  }
}

bool BeautyFaceUnitFilter::DoRender(bool updateSinks) {
  // 检查查找表图像是否已设置
  if (!gray_image_ || !original_image_ || !skin_image_ || !custom_image_) {
    // 如果查找表图像未设置，则直接透传原图
    return Source::DoRender(updateSinks);
  }

  static const float imageVertices[] = {
      -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f,
  };

  GPUPixelContext::GetInstance()->SetActiveGlProgram(filter_program_);
  framebuffer_->Activate();
  GL_CALL(glClearColor(background_color_.r, background_color_.g,
                       background_color_.b, background_color_.a));
  GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

  GL_CALL(glActiveTexture(GL_TEXTURE2));
  GL_CALL(glBindTexture(GL_TEXTURE_2D,
                        input_framebuffers_[0].frame_buffer->GetTexture()));
  filter_program_->SetUniformValue("inputImageTexture", 2);

  GL_CALL(glActiveTexture(GL_TEXTURE3));
  GL_CALL(glBindTexture(GL_TEXTURE_2D,
                        input_framebuffers_[1].frame_buffer->GetTexture()));
  filter_program_->SetUniformValue("inputImageTexture2", 3);

  GL_CALL(glActiveTexture(GL_TEXTURE4));
  GL_CALL(glBindTexture(GL_TEXTURE_2D,
                        input_framebuffers_[2].frame_buffer->GetTexture()));
  filter_program_->SetUniformValue("inputImageTexture3", 4);

  // texcoord attribute
  uint32_t filter_tex_coord_attribute =
      filter_program_->GetAttribLocation("inputTextureCoordinate");
  GL_CALL(glEnableVertexAttribArray(filter_tex_coord_attribute));
  GL_CALL(glVertexAttribPointer(
      filter_tex_coord_attribute, 2, GL_FLOAT, 0, 0,
      GetTextureCoordinate(input_framebuffers_[0].rotation_mode)));

  glActiveTexture(GL_TEXTURE5);
  glBindTexture(GL_TEXTURE_2D, gray_image_->GetFramebuffer()->GetTexture());
  filter_program_->SetUniformValue("lookUpGray", 5);

  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, original_image_->GetFramebuffer()->GetTexture());
  filter_program_->SetUniformValue("lookUpOrigin", 6);

  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, skin_image_->GetFramebuffer()->GetTexture());
  filter_program_->SetUniformValue("lookUpSkin", 7);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, custom_image_->GetFramebuffer()->GetTexture());
  filter_program_->SetUniformValue("lookUpCustom", 0);

  float width_offset = 1.0 / this->GetRotatedFramebufferWidth();
  float height_offset = 1.0 / this->GetRotatedFramebufferHeight();
  filter_program_->SetUniformValue("widthOffset", width_offset);
  filter_program_->SetUniformValue("heightOffset", height_offset);

  // vertex position
  GL_CALL(glVertexAttribPointer(filter_position_attribute_, 2, GL_FLOAT, 0, 0,
                                imageVertices));

  filter_program_->SetUniformValue("sharpen", sharpen_factor_);
  filter_program_->SetUniformValue("blurAlpha", blur_alpha_);
  filter_program_->SetUniformValue("whiten", white_balance_);
  filter_program_->SetUniformValue("warmth", white_warmth_);
  filter_program_->SetUniformValue("ruddy", ruddy_factor_);

  // draw
  GL_CALL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

  framebuffer_->Deactivate();

  return Source::DoRender(updateSinks);
}

void BeautyFaceUnitFilter::SetSharpen(float sharpen) {
  sharpen_factor_ = sharpen;
}

void BeautyFaceUnitFilter::SetBlurAlpha(float blur_alpha) {
  blur_alpha_ = blur_alpha;
}

void BeautyFaceUnitFilter::SetWhite(float white, float warmth) {
  // #if defined(GPUPIXEL_MAC)
  //   white_balance_ = white / 2.5;
  // #else
  //   white_balance_ = white;
  // #endif
  //   white_warmth_ = warmth;
  white_balance_ = 0.2;
  ruddy_factor_ = white;
}

void BeautyFaceUnitFilter::SetRuddy(float ruddy) {
  ruddy_factor_ = ruddy;
}

}  // namespace gpupixel
