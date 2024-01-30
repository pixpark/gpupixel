#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "gpupixel.h"
using namespace gpupixel;


std::shared_ptr<BeautyFaceFilter> beauty_face_filter_;
std::shared_ptr<FaceReshapeFilter> face_reshape_filter_;
std::shared_ptr<gpupixel::FaceMakeupFilter> lipstick_filter_;
std::shared_ptr<gpupixel::FaceMakeupFilter> blusher_filter_;
std::shared_ptr<SourceImage> gpuSourceImage;
std::shared_ptr<TargetRawDataOutput> output_;
std::shared_ptr<TargetView> target_view;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

 void error_callback( int error, const char *msg ) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}

int main()
{

     GLFWwindow* window = GPUPixelContext::getInstance()->GetGLContext();
     glfwSetErrorCallback( error_callback );
    
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwShowWindow(window);

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
 
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    
    // create filter
    // ----
 #if 0
    auto mouth = SourceImage::create("mouth.png");
    lipstick_filter_ = FaceMakeupFilter::create();
    lipstick_filter_->setImageTexture(mouth);
    lipstick_filter_->setTextureBounds(FrameBounds{502.5, 710, 262.5, 167.5});

    auto blusher = SourceImage::create("blusher.png");
    blusher_filter_ = FaceMakeupFilter::create();
    blusher_filter_->setImageTexture(blusher);
    blusher_filter_->setTextureBounds(FrameBounds{395, 520, 489, 209});
 #endif
    beauty_face_filter_ = BeautyFaceFilter::create();
 
    //  filter pipline
    // ----
    gpuSourceImage = SourceImage::create("demo.jpg");
    target_view = std::make_shared<TargetView>();
    
    gpuSourceImage->addTarget(beauty_face_filter_)
                  ->addTarget(target_view); 
   
    // set default value
    // ----
    beauty_face_filter_->setBlurAlpha(9.0f/10);
    beauty_face_filter_->setWhite(9.0f/20);

    // 
    target_view->onSizeChanged(1280, 720);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        
        // 
        // -----
        gpuSourceImage->proceed();

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    
}
