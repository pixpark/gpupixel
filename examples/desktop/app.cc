#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "gpupixel.h"
using namespace gpupixel;

std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
std::shared_ptr<SourceImage> gpuSourceImage;
std::shared_ptr<TargetRawDataOutput> output_;
std::shared_ptr<TargetView> target_view;

float beautyValue = 0;
float whithValue = 0;
float thinFaceValue = 0;
float bigeyeValue = 0;
float lipstickValue = 0;
float blusherValue = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

void error_callback(int error, const char* msg) {
  std::string s;
  s = " [" + std::to_string(error) + "] " + msg + '\n';
  std::cerr << s << std::endl;
}

int main() {
  glfwInit();
  GLFWwindow* window = GPUPixelContext::getInstance()->GetGLContext();

  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }

  gladLoadGL();
  glfwMakeContextCurrent(window);

  glfwShowWindow(window);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
 
  // ----
  //  filter pipline
  // ----
  gpuSourceImage = SourceImage::create("demo.png");
  target_view = std::make_shared<TargetView>();
 
  beauty_face_filter_ = BeautyFaceFilter::create();
 
  gpuSourceImage->addTarget(beauty_face_filter_);
  beauty_face_filter_->addTarget(target_view);

  //
  target_view->onSizeChanged(1280, 720);

  // render loop
  // -----------
  while (!glfwWindowShouldClose(window)) {
    // input
    // -----
    processInput(window);

    //
    // -----
    gpuSourceImage->Render();

    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved
    // etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // glfw: terminate, clearing all previously allocated GLFW resources.
  // ------------------------------------------------------------------
  glfwTerminate();
  return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this
// frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, true);
  }

  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
    beautyValue++;
    if (beautyValue > 10.0) {
      beautyValue = 10.0;
    }
    beauty_face_filter_->setBlurAlpha(beautyValue / 10);
  }

  if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
    beautyValue--;
    if (beautyValue < 0.0) {
      beautyValue = 0.0;
    }
    beauty_face_filter_->setBlurAlpha(beautyValue / 10);
  }

  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
    whithValue++;
    if (whithValue > 10.0) {
      whithValue = 10.0;
    }
    beauty_face_filter_->setWhite(whithValue / 20);
  }

  if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
    whithValue--;
    if (whithValue < 0.0) {
      whithValue = 0.0;
    }
    beauty_face_filter_->setWhite(whithValue / 20);
  }
}

// glfw: whenever the window size changed (by OS or user resize) this callback
// function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  // make sure the viewport matches the new window dimensions; note that width
  // and height will be significantly larger than specified on retina displays.
  glViewport(0, 0, width, height);
}
