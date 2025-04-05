#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "face_detector.h"
#include "gpupixel.h"
#include "imgui.h"

using namespace gpupixel;

// Filters
std::shared_ptr<BeautyFaceFilter> beauty_filter_;
std::shared_ptr<FaceReshapeFilter> reshape_filter_;
std::shared_ptr<gpupixel::LipstickFilter> lipstick_filter_;
std::shared_ptr<gpupixel::BlusherFilter> blusher_filter_;
std::shared_ptr<SourceImage> source_image_;
std::shared_ptr<SinkRender> render_sink_;
std::shared_ptr<FaceDetector> face_detector_;

// Filter parameters
float beauty_strength_ = 0.0f;
float whitening_strength_ = 0.0f;
float face_slim_strength_ = 0.0f;
float eye_enlarge_strength_ = 0.0f;
float lipstick_strength_ = 0.0f;
float blusher_strength_ = 0.0f;

// GLFW window handle
GLFWwindow* main_window_ = nullptr;

// GLFW framebuffer resize callback
void OnFramebufferResize(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// Initialize GLFW and create window
bool SetupGlfwWindow() {
  // Initialize GLFW
  if (!glfwInit()) {
    std::cout << "Failed to initialize GLFW" << std::endl;
    return false;
  }

  // Get OpenGL context window from GPUPixel
  main_window_ = GPUPixelContext::GetInstance()->GetGLContext();
  if (main_window_ == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }

  // Initialize GLAD and setup window parameters
  gladLoadGL();
  glfwMakeContextCurrent(main_window_);
  glfwShowWindow(main_window_);
  glfwSetFramebufferSizeCallback(main_window_, OnFramebufferResize);

  return true;
}

// Initialize ImGui interface
void SetupImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(main_window_, true);
  ImGui_ImplOpenGL3_Init("#version 130");
}

// Initialize GPUPixel filters and pipeline
void SetupFilterPipeline() {
  // Create filters
  lipstick_filter_ = LipstickFilter::Create();
  blusher_filter_ = BlusherFilter::Create();
  reshape_filter_ = FaceReshapeFilter::Create();
  beauty_filter_ = BeautyFaceFilter::Create();
  face_detector_ = std::make_shared<FaceDetector>();

  // Create source image and render sink
  source_image_ = SourceImage::Create("demo.png");
  render_sink_ = std::make_shared<SinkRender>();

  // Build filter pipeline
  source_image_->AddSink(lipstick_filter_)
      ->AddSink(blusher_filter_)
      ->AddSink(reshape_filter_)
      ->AddSink(beauty_filter_)
      ->AddSink(render_sink_);

  render_sink_->SetRenderSize(1280, 720);
}

// Update filter parameters from UI controls
void UpdateFilterParameters() {
  // Beauty filter controls
  if (ImGui::SliderFloat("Smoothing", &beauty_strength_, 0.0f, 10.0f)) {
    beauty_filter_->SetBlurAlpha(beauty_strength_ / 10.0f);
  }

  if (ImGui::SliderFloat("Whitening", &whitening_strength_, 0.0f, 10.0f)) {
    beauty_filter_->SetWhite(whitening_strength_ / 20.0f);
  }

  if (ImGui::SliderFloat("Face Slimming", &face_slim_strength_, 0.0f, 10.0f)) {
    reshape_filter_->SetFaceSlimLevel(face_slim_strength_ / 200.0f);
  }

  if (ImGui::SliderFloat("Eye Enlarging", &eye_enlarge_strength_, 0.0f,
                         10.0f)) {
    reshape_filter_->SetEyeZoomLevel(eye_enlarge_strength_ / 100.0f);
  }

  if (ImGui::SliderFloat("Lipstick", &lipstick_strength_, 0.0f, 10.0f)) {
    lipstick_filter_->SetBlendLevel(lipstick_strength_ / 10.0f);
  }

  if (ImGui::SliderFloat("Blusher", &blusher_strength_, 0.0f, 10.0f)) {
    blusher_filter_->SetBlendLevel(blusher_strength_ / 10.0f);
  }
}

// Render a single frame
void RenderFrame() {
  // Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Create control panel
  ImGui::Begin("Beauty Control Panel", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize);
  UpdateFilterParameters();
  ImGui::End();

  int width = source_image_->GetWidth();
  int height = source_image_->GetHeight();
  const unsigned char* buffer = source_image_->GetRgbaImageBuffer();

  std::vector<float> landmarks =
      face_detector_->Detect(buffer, width, height, width, GPUPIXEL_MODE_FMT_PICTURE,
                             GPUPIXEL_FRAME_TYPE_RGBA);

  if (!landmarks.empty()) {
    lipstick_filter_->SetFaceLandmarks(landmarks);
    blusher_filter_->SetFaceLandmarks(landmarks);
    reshape_filter_->SetFaceLandmarks(landmarks);
  }

  // Process and render image
  source_image_->Render();

  // Render ImGui
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, 1280, 720);
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  // Swap buffers and poll events
  glfwSwapBuffers(main_window_);
  glfwPollEvents();
}

// Clean up resources
void CleanupResources() {
  // Cleanup ImGui
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  // Cleanup GLFW
  glfwTerminate();
}

int main() {
  // Initialize window and OpenGL context
  if (!SetupGlfwWindow()) {
    return -1;
  }

  // Setup ImGui interface
  SetupImGui();

  // Initialize filters and pipeline
  SetupFilterPipeline();

  // Main render loop
  while (!glfwWindowShouldClose(main_window_)) {
    RenderFrame();
  }

  // Cleanup
  CleanupResources();

  return 0;
}
