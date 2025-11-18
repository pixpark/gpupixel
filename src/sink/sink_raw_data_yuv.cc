#include "gpupixel/sink/sink_raw_data.h"
#include <cstring>
#include "core/gpupixel_context.h"
#include "utils/util.h"

#ifdef __ARM_NEON
#include <arm_neon.h>
#endif

namespace gpupixel {

inline void neon_memcpy_64(void *dest, const void *src, size_t n);

const std::string kI420VertexShaderString = R"(
    attribute vec4 position;
    attribute vec4 input_tc;
    varying vec2 tc;

    void main() {
      gl_Position = position;
      tc = input_tc.xy;
    }
)";

const std::string kI420FragmentShaderString =
#if defined(GPUPIXEL_GLES_SHADER)
R"(precision highp float;)"
#endif
R"(
    varying vec2 tc;
    uniform sampler2D tex;
    uniform vec2 px;
    uniform vec4 rgb2yuv;

    void main() {
      gl_FragColor.r = rgb2yuv.a + dot(rgb2yuv.rgb, texture2D(tex, tc - 1.5 * px).rgb);
      gl_FragColor.g = rgb2yuv.a + dot(rgb2yuv.rgb, texture2D(tex, tc - 0.5 * px).rgb);
      gl_FragColor.b = rgb2yuv.a + dot(rgb2yuv.rgb, texture2D(tex, tc + 0.5 * px).rgb);
      gl_FragColor.a = rgb2yuv.a + dot(rgb2yuv.rgb, texture2D(tex, tc + 1.5 * px).rgb);
    }
)";

std::shared_ptr<SinkRawYuv> SinkRawYuv::Create() {
  std::shared_ptr<SinkRawYuv> ret;
  gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext(
      [&] { ret = std::shared_ptr<SinkRawYuv>(new SinkRawYuv()); });
  return ret;
}

SinkRawYuv::SinkRawYuv() {
  InitWithShaderString(kI420VertexShaderString,
                       kI420FragmentShaderString);
}

SinkRawYuv::~SinkRawYuv() {
    // Clean up YUV frame buffer
    delete[] yuv_buffer_;
    yuv_buffer_ = nullptr;
    delete[] uv_buffer_;
    uv_buffer_ = nullptr;
}

void SinkRawYuv::Render() {
  if (input_framebuffers_.empty()) {
    return;
  }

  int width = input_framebuffers_[0].frame_buffer->GetWidth();
  int height = input_framebuffers_[0].frame_buffer->GetHeight();

  if (width_ != width || height_ != height) {
    width_ = width;
    height_ = height;
    InitFramebuffer(width_ / 4, height_ * 3 / 2);
    InitOutputBuffer(width, height);
  }
  int fb_width = framebuffer_->GetWidth();
  int fb_height = framebuffer_->GetHeight();

  GPUPixelContext::GetInstance()->SetActiveGlProgram(shader_program_);
  framebuffer_->Activate();

  glViewport(0, 0, fb_width, fb_height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  float image_vertices[] = {
      -1.0, -1.0,  // Bottom left
      1.0,  -1.0,  // Bottom right
      -1.0, 1.0,   // Top left
      1.0,  1.0    // Top right
  };

  float texture_vertices[] = {
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
  };

  GL_CALL(glEnableVertexAttribArray(position_attribute_));
  GL_CALL(glVertexAttribPointer(position_attribute_, 2, GL_FLOAT, 0, 0,image_vertices));

  GL_CALL(glEnableVertexAttribArray(tex_coord_attribute_));
  GL_CALL(glVertexAttribPointer(tex_coord_attribute_, 2, GL_FLOAT, 0, 0,texture_vertices));

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D,input_framebuffers_[0].frame_buffer->GetTexture());

  GL_CALL(shader_program_->SetUniformValue("sTexture", 0));

  int y_height = height_;
  int uv_height = fb_height - height_;
  // y
  glViewport(0, 0, fb_width, y_height);
  shader_program_->SetUniformValue("px", Vector2(1.0f / (float) width_, 0.0f));
  shader_program_->SetUniformValue("rgb2yuv", 0.299f, 0.587f, 0.114f, 0.0f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  // u
  glViewport(0, y_height, fb_width / 2, uv_height);
  shader_program_->SetUniformValue("px", Vector2(2.0f / (float) width_, 0.0f));
  shader_program_->SetUniformValue("rgb2yuv", -0.168736f, -0.331264f, 0.5f, 0.5f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  // v
  glViewport(fb_width / 2, y_height, fb_width / 2, uv_height);
  shader_program_->SetUniformValue("rgb2yuv", 0.5f, -0.418688f, -0.081312f, 0.5f);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  framebuffer_->Deactivate();
}

bool SinkRawYuv::InitWithShaderString(
    const std::string& vertex_shader_source,
    const std::string& fragment_shader_source) {
  shader_program_ = GPUPixelGLProgram::CreateWithShaderString(
      vertex_shader_source, fragment_shader_source);
  GPUPixelContext::GetInstance()->SetActiveGlProgram(shader_program_);
  position_attribute_ = shader_program_->GetAttribLocation("position");
  tex_coord_attribute_ = shader_program_->GetAttribLocation("input_tc");

  return true;
}

int SinkRawYuv::RenderToOutput(uint8_t* buffer) {
    if (!framebuffer_) {
        return -1;
    }
    framebuffer_->Activate();

    GL_CALL(glReadPixels(0, 0, framebuffer_->GetWidth(), framebuffer_->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, buffer));

    int y_width = width_;
    int y_height = height_;
    int uv_width = width_ / 2;
    int uv_height = height_ / 2;
    auto buffer_uv = buffer + y_width * y_height;
    auto u_size = uv_width * uv_height;

    neon_memcpy_64(uv_buffer_, buffer_uv, u_size * 2);

    for (int i = 0; i < uv_height; i++) {
        auto stride = i * y_width;
        auto src_u = uv_buffer_ + stride;
        auto src_v = uv_buffer_ + stride + uv_width;

        auto step = i * uv_width;
        auto dst_u = buffer_uv + step;
        auto dst_v = buffer_uv + step + u_size;

        neon_memcpy_64(dst_u, src_u, uv_width);
        neon_memcpy_64(dst_v, src_v, uv_width);
    }
    framebuffer_->Deactivate();
    return 0;
}



const uint8_t* SinkRawYuv::GetI420Buffer() {
    GetI420Buffer(yuv_buffer_);
    return yuv_buffer_;
}

void SinkRawYuv::GetI420Buffer(uint8_t *input_buffer) {
    if (input_buffer != yuv_buffer_) {
        delete[] yuv_buffer_;
        yuv_buffer_ = nullptr;
    }
    gpupixel::GPUPixelContext::GetInstance()->SyncRunWithContext(
      [=] { RenderToOutput(input_buffer); });
}

void SinkRawYuv::InitOutputBuffer(int width, int height) {
    delete[] yuv_buffer_;
    uint32_t yuv_size = width * height * 3 / 2;
    yuv_buffer_ = new uint8_t[yuv_size];
    std::memset(yuv_buffer_, 0, yuv_size);

    delete[] uv_buffer_;
    uint32_t uv_size = width * height / 2;
    uv_buffer_ = new uint8_t[uv_size];
    std::memset(uv_buffer_, 0, uv_size);
}

void SinkRawYuv::InitFramebuffer(int width, int height) {
  if (!framebuffer_ || (framebuffer_->GetWidth() != width ||
                        framebuffer_->GetHeight() != height)) {

    framebuffer_ = GPUPixelContext::GetInstance()
                       ->GetFramebufferFactory()
                       ->CreateFramebuffer(width, height);
  }
}

inline void neon_memcpy_64(void *dest, const void *src, size_t n) {
#ifdef __ARM_NEON
    auto* dst = (uint8_t*) dest;
    const auto* src_const = (const uint8_t*) src;
    size_t i = 0;

    for (; i <= n - 64; i += 64) {
        // Load 4x 16-byte vectors
        uint8x16_t v0 = vld1q_u8(src_const + i);
        uint8x16_t v1 = vld1q_u8(src_const + i + 16);
        uint8x16_t v2 = vld1q_u8(src_const + i + 32);
        uint8x16_t v3 = vld1q_u8(src_const + i + 48);

        // Store 4x 16-byte vectors
        vst1q_u8(dst + i,      v0);
        vst1q_u8(dst + i + 16, v1);
        vst1q_u8(dst + i + 32, v2);
        vst1q_u8(dst + i + 48, v3);
    }

    for (; i <= n - 16; i += 16) {
        uint8x16_t v = vld1q_u8(src_const + i);
        vst1q_u8(dst + i, v);
    }

    for (; i < n; i++) {
        dst[i] = src_const[i];
    }
#else
        memcpy(dest, src, n);
#endif
}

}
