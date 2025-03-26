#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "gpupixel.h"
using namespace gpupixel;


std::shared_ptr<BeautyFaceFilter> beautyFaceFilter;
std::shared_ptr<FaceReshapeFilter> faceReshapeFilter;
std::shared_ptr<gpupixel::LipstickFilter> lipstickFilter;
std::shared_ptr<gpupixel::BlusherFilter> blusherFilter;
std::shared_ptr<SourceImage> sourceImage;
std::shared_ptr<SinkRawData> sinkRawData;
std::shared_ptr<SinkRender> sinkRender;

float beautyIntensity = 0;
float whitenIntensity = 0;
float faceSlimIntensity = 0;
float eyeEnlargeIntensity = 0;
float lipstickIntensity = 0;
float blusherIntensity = 0;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

 void error_callback( int error, const char *msg ) {
    std::string s;
    s = " [" + std::to_string(error) + "] " + msg + '\n';
    std::cerr << s << std::endl;
}

int main()
{
    glfwInit();
    GLFWwindow* window = GPUPixelContext::getInstance()->GetGLContext();
  
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    gladLoadGL();
    glfwMakeContextCurrent(window);

   glfwShowWindow(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    // create filter
    // ----
    lipstickFilter = LipstickFilter::create();
    blusherFilter = BlusherFilter::create();
    faceReshapeFilter = FaceReshapeFilter::create();
    
    //  filter pipline
    // ----
    sourceImage = SourceImage::create("demo.png");
    sinkRender = std::make_shared<SinkRender>();

    sourceImage->RegLandmarkCallback([=](std::vector<float> landmarks) {
       lipstickFilter->SetFaceLandmarks(landmarks);
       blusherFilter->SetFaceLandmarks(landmarks);
       faceReshapeFilter->SetFaceLandmarks(landmarks);
     });

    beautyFaceFilter = BeautyFaceFilter::create();
 
    sourceImage->addSink(lipstickFilter)
                ->addSink(blusherFilter)
                ->addSink(faceReshapeFilter)
                ->addSink(beautyFaceFilter)
                ->addSink(sinkRender);
                    
    // 
    sinkRender->onSizeChanged(1280, 720);
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);
        
        // 
        // -----
        sourceImage->Render();

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

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        beautyIntensity++;
        if(beautyIntensity > 10.0) beautyIntensity = 10.0;
        beautyFaceFilter->setBlurAlpha(beautyIntensity/10);
    } 

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        beautyIntensity--;
        if(beautyIntensity < 0.0) beautyIntensity = 0.0;
        beautyFaceFilter->setBlurAlpha(beautyIntensity/10);
    } 


    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        whitenIntensity++;
        if(whitenIntensity > 10.0) whitenIntensity = 10.0;
        beautyFaceFilter->setWhite(whitenIntensity/20);
    }

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        whitenIntensity--;
        if(whitenIntensity < 0.0) whitenIntensity = 0.0;
        beautyFaceFilter->setWhite(whitenIntensity/20);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        faceSlimIntensity++;
        if(faceSlimIntensity > 10.0) faceSlimIntensity = 10.0;
        faceReshapeFilter->setFaceSlimLevel(faceSlimIntensity/200);
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        faceSlimIntensity--;
        if(faceSlimIntensity < 0.0) faceSlimIntensity = 0.0;
        faceReshapeFilter->setFaceSlimLevel(faceSlimIntensity/200);
    }

    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        eyeEnlargeIntensity++;
        if(eyeEnlargeIntensity > 10.0) eyeEnlargeIntensity = 10.0;
        faceReshapeFilter->setEyeZoomLevel(eyeEnlargeIntensity/100);
    }

    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        eyeEnlargeIntensity--;
        if(eyeEnlargeIntensity < 0.0) eyeEnlargeIntensity = 0.0;
        faceReshapeFilter->setEyeZoomLevel(eyeEnlargeIntensity/100);
    }

    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        lipstickIntensity++;
        if(lipstickIntensity > 10.0) lipstickIntensity = 10.0;
        lipstickFilter->setBlendLevel(lipstickIntensity/10); 
    }

    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        lipstickIntensity--;
        if(lipstickIntensity < 0.0) lipstickIntensity = 0.0;
        lipstickFilter->setBlendLevel(lipstickIntensity/10); 
    }

    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        blusherIntensity++;
        if(blusherIntensity > 10.0) blusherIntensity = 10.0;
        blusherFilter->setBlendLevel(blusherIntensity/10); 
    }

    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        blusherIntensity--;
        if(blusherIntensity < 0.0) blusherIntensity = 0.0;
        blusherFilter->setBlendLevel(blusherIntensity/10); 
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    
}
