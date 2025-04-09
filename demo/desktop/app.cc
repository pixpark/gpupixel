#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "face_detector.h"
#include "gpupixel.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace gpupixel;

// Filters
std::shared_ptr<BeautyFaceFilter> beauty_filter_;
std::shared_ptr<FaceReshapeFilter> reshape_filter_;
std::shared_ptr<gpupixel::LipstickFilter> lipstick_filter_;
std::shared_ptr<gpupixel::BlusherFilter> blusher_filter_;
std::shared_ptr<SourceImage> source_image_;
std::shared_ptr<SinkRawData> sink_raw_data_;
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

// Check shader compilation/linking errors
bool CheckShaderErrors(GLuint shader, const char* type) {
  GLint success;
  GLchar infoLog[1024];

  if (strcmp(type, "PROGRAM") != 0) {
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(shader, 1024, NULL, infoLog);
      std::cerr << "Shader compilation error: " << type << "\n"
                << infoLog << std::endl;
      return false;
    }
  } else {
    glGetProgramiv(shader, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(shader, 1024, NULL, infoLog);
      std::cerr << "Program linking error: " << type << "\n"
                << infoLog << std::endl;
      return false;
    }
  }
  return true;
}

// GLFW framebuffer resize callback
void OnFramebufferResize(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

// GLFW error callback
void ErrorCallback(int error, const char* description) {
  std::cerr << "GLFW Error: " << description << std::endl;
}

// Initialize GLFW and create window
bool SetupGlfwWindow() {
  // Set GLFW error callback
  glfwSetErrorCallback(ErrorCallback);

  // Initialize GLFW
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return false;
  }

  // Create window with compatibility profile for better support
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
  glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

  main_window_ = glfwCreateWindow(1280, 720, "GPUPixel Demo", NULL, NULL);
  if (main_window_ == NULL) {
    std::cerr << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return false;
  }

  // Initialize GLAD and setup window parameters
  glfwMakeContextCurrent(main_window_);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    glfwDestroyWindow(main_window_);
    glfwTerminate();
    return false;
  }

  glfwSetFramebufferSizeCallback(main_window_, OnFramebufferResize);

  return true;
}

// Initialize ImGui
void SetupImGui() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(main_window_, true);

  // Use GLSL 1.3 for compatibility
  const char* glsl_version = "#version 130";
  ImGui_ImplOpenGL3_Init(glsl_version);
}

// Initialize GPUPixel filters and pipeline
void SetupFilterPipeline() {
  // Create filters
  lipstick_filter_ = LipstickFilter::Create();
  blusher_filter_ = BlusherFilter::Create();
  reshape_filter_ = FaceReshapeFilter::Create();
  beauty_filter_ = BeautyFaceFilter::Create();
  face_detector_ = FaceDetector::Create();

  // Create source image and sink raw data
  source_image_ = SourceImage::Create("demo.png");
  sink_raw_data_ = SinkRawData::Create();

  // Build filter pipeline
  source_image_->AddSink(lipstick_filter_)
      ->AddSink(blusher_filter_)
      ->AddSink(reshape_filter_)
      ->AddSink(beauty_filter_)
      ->AddSink(sink_raw_data_);
}

// Update filter parameters from ImGui controls
void UpdateFilterParametersFromUI() {
  ImGui::Begin("Beauty Control Panel", nullptr,
               ImGuiWindowFlags_AlwaysAutoResize);

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

  ImGui::End();
}

// Update filter parameters directly
void UpdateFilterParameters() {
  beauty_filter_->SetBlurAlpha(beauty_strength_ / 10.0f);
  beauty_filter_->SetWhite(whitening_strength_ / 20.0f);

  // Face reshape filter controls
  reshape_filter_->SetFaceSlimLevel(face_slim_strength_ / 200.0f);
  reshape_filter_->SetEyeZoomLevel(eye_enlarge_strength_ / 100.0f);

  // Lipstick and blusher filter controls
  lipstick_filter_->SetBlendLevel(lipstick_strength_ / 10.0f);
  blusher_filter_->SetBlendLevel(blusher_strength_ / 10.0f);
}

// Render RGBA data to screen
void RenderRGBAToScreen(const uint8_t* rgba_data, int width, int height) {
  if (!rgba_data || width <= 0 || height <= 0) {
    return;
  }

  // Render the RGBA data to the screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, 1280, 720);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Create a texture from the RGBA data
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, rgba_data);

  // Use GLSL 1.2 for maximum compatibility
  const char* vertexShaderSource = R"(
    #version 120
    attribute vec3 aPos;
    attribute vec2 aTexCoord;
    varying vec2 TexCoord;
    void main() {
      gl_Position = vec4(aPos, 1.0);
      TexCoord = aTexCoord;
    }
  )";

  const char* fragmentShaderSource = R"(
    #version 120
    varying vec2 TexCoord;
    uniform sampler2D texture1;
    void main() {
      gl_FragColor = texture2D(texture1, TexCoord);
    }
  )";

  // Compile shaders
  GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);
  if (!CheckShaderErrors(vertexShader, "VERTEX")) {
    return;
  }

  GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);
  if (!CheckShaderErrors(fragmentShader, "FRAGMENT")) {
    return;
  }

  // Create shader program
  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);
  if (!CheckShaderErrors(shaderProgram, "PROGRAM")) {
    return;
  }

  // Use shader program
  glUseProgram(shaderProgram);

  // Calculate aspect ratio and adjust vertices to maintain aspect ratio
  float imageAspectRatio = (float)width / (float)height;
  float windowAspectRatio = 1280.0f / 720.0f;

  float scaleX = 1.0f;
  float scaleY = 1.0f;

  if (imageAspectRatio > windowAspectRatio) {
    // Image is wider than window
    scaleY = windowAspectRatio / imageAspectRatio;
  } else {
    // Image is taller than window
    scaleX = imageAspectRatio / windowAspectRatio;
  }

  // Set up vertex data and buffers with corrected aspect ratio
  float vertices[] = {
      // positions        // texture coords
      -scaleX, -scaleY, 0.0f,
      0.0f,    1.0f,  // Bottom left (flipped Y texture coord)
      scaleX,  -scaleY, 0.0f,
      1.0f,    1.0f,  // Bottom right (flipped Y texture coord)
      scaleX,  scaleY,  0.0f,
      1.0f,    0.0f,  // Top right (flipped Y texture coord)
      -scaleX, scaleY,  0.0f,
      0.0f,    0.0f  // Top left (flipped Y texture coord)
  };

  unsigned int indices[] = {0, 1, 2, 2, 3, 0};

  GLuint VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glBindVertexArray(VAO);

  glGenBuffers(1, &VBO);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glGenBuffers(1, &EBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  // Position attribute
  GLint posAttrib = glGetAttribLocation(shaderProgram, "aPos");
  glEnableVertexAttribArray(posAttrib);
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)0);

  // Texture coord attribute
  GLint texAttrib = glGetAttribLocation(shaderProgram, "aTexCoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                        (void*)(3 * sizeof(float)));

  // Set texture uniform
  GLint texUniform = glGetUniformLocation(shaderProgram, "texture1");
  glUniform1i(texUniform, 0);

  // Draw
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, texture);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

  // Clean up
  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);
  glDeleteTextures(1, &texture);
  glDeleteProgram(shaderProgram);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
}

// Render a single frame
void RenderFrame() {
  // Start ImGui frame
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  // Update filter parameters from UI
  UpdateFilterParametersFromUI();

  int width = source_image_->GetWidth();
  int height = source_image_->GetHeight();
  const unsigned char* buffer = source_image_->GetRgbaImageBuffer();

  // Detect face landmarks
  std::vector<float> landmarks = face_detector_->Detect(
      buffer, width, height, width * 4, GPUPIXEL_MODE_FMT_PICTURE,
      GPUPIXEL_FRAME_TYPE_RGBA);

  if (!landmarks.empty()) {
    lipstick_filter_->SetFaceLandmarks(landmarks);
    blusher_filter_->SetFaceLandmarks(landmarks);
    reshape_filter_->SetFaceLandmarks(landmarks);
  }

  // Process image
  source_image_->Render();

  // Get processed RGBA data
  const uint8_t* rgba_data = sink_raw_data_->GetRgbaBuffer();
  width = sink_raw_data_->GetWidth();
  height = sink_raw_data_->GetHeight();

  // Render RGBA data to screen using the encapsulated function
  RenderRGBAToScreen(rgba_data, width, height);

  // Render ImGui
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
  if (main_window_) {
    glfwDestroyWindow(main_window_);
    main_window_ = nullptr;
  }
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
    // Render frame
    RenderFrame();
  }

  // Cleanup
  CleanupResources();

  return 0;
}
