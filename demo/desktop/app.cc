#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include "gpupixel.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

using namespace gpupixel;

// Filters
std::shared_ptr<BeautyFaceFilter> beautyFilter;
std::shared_ptr<FaceReshapeFilter> reshapeFilter;
std::shared_ptr<gpupixel::LipstickFilter> lipstickFilter;
std::shared_ptr<gpupixel::BlusherFilter> blusherFilter;
std::shared_ptr<SourceImage> sourceImage;
std::shared_ptr<SinkRender> renderSink;

// Filter parameters
float beautyStrength = 0.0f;
float whiteningStrength = 0.0f;
float faceSlimStrength = 0.0f;
float eyeEnlargeStrength = 0.0f;
float lipstickStrength = 0.0f;
float blusherStrength = 0.0f;

// GLFW window handle
GLFWwindow* mainWindow = nullptr;

// GLFW framebuffer resize callback
void onFramebufferResize(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// Initialize GLFW and create window
bool setupGlfwWindow()
{
    // Initialize GLFW
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return false;
    }
    
    // Get OpenGL context window from GPUPixel
    mainWindow = GPUPixelContext::GetInstance()->GetGLContext();
    if (mainWindow == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    // Initialize GLAD and setup window parameters
    gladLoadGL();
    glfwMakeContextCurrent(mainWindow);
    glfwShowWindow(mainWindow);
    glfwSetFramebufferSizeCallback(mainWindow, onFramebufferResize);
    
    return true;
}

// Initialize ImGui interface
void setupImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

// Initialize GPUPixel filters and pipeline
void setupFilterPipeline()
{
    // Create filters
    lipstickFilter = LipstickFilter::Create();
    blusherFilter = BlusherFilter::Create();
    reshapeFilter = FaceReshapeFilter::Create();
    beautyFilter = BeautyFaceFilter::Create();
    
    // Create source image and render sink
    sourceImage = SourceImage::Create("demo.png");
    renderSink = std::make_shared<SinkRender>();

    // Setup face landmarks callback
    sourceImage->RegLandmarkCallback([=](std::vector<float> landmarks) {
       lipstickFilter->SetFaceLandmarks(landmarks);
       blusherFilter->SetFaceLandmarks(landmarks);
       reshapeFilter->SetFaceLandmarks(landmarks);
    });

    // Build filter pipeline
    sourceImage->AddSink(lipstickFilter)
                ->AddSink(blusherFilter)
                ->AddSink(reshapeFilter)
                ->AddSink(beautyFilter)
                ->AddSink(renderSink);
                    
    renderSink->SetRenderSize(1280, 720);
}

// Update filter parameters from UI controls
void updateFilterParameters()
{
    // Beauty filter controls
    if (ImGui::SliderFloat("Smoothing", &beautyStrength, 0.0f, 10.0f)) {
        beautyFilter->SetBlurAlpha(beautyStrength/10.0f);
    }
    
    if (ImGui::SliderFloat("Whitening", &whiteningStrength, 0.0f, 10.0f)) {
        beautyFilter->SetWhite(whiteningStrength/20.0f);
    }
    
    if (ImGui::SliderFloat("Face Slimming", &faceSlimStrength, 0.0f, 10.0f)) {
        reshapeFilter->SetFaceSlimLevel(faceSlimStrength/200.0f);
    }
    
    if (ImGui::SliderFloat("Eye Enlarging", &eyeEnlargeStrength, 0.0f, 10.0f)) {
        reshapeFilter->SetEyeZoomLevel(eyeEnlargeStrength/100.0f);
    }
    
    if (ImGui::SliderFloat("Lipstick", &lipstickStrength, 0.0f, 10.0f)) {
        lipstickFilter->SetBlendLevel(lipstickStrength/10.0f);
    }
    
    if (ImGui::SliderFloat("Blusher", &blusherStrength, 0.0f, 10.0f)) {
        blusherFilter->SetBlendLevel(blusherStrength/10.0f);
    }
}

// Render a single frame
void renderFrame()
{
    // Start ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    // Create control panel
    ImGui::Begin("Beauty Control Panel", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
    updateFilterParameters();
    ImGui::End();
    
    // Process and render image
    sourceImage->Render();
    
    // Render ImGui
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1280, 720);
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    // Swap buffers and poll events
    glfwSwapBuffers(mainWindow);
    glfwPollEvents();
}

// Clean up resources
void cleanupResources()
{
    // Cleanup ImGui
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    // Cleanup GLFW
    glfwTerminate();
}

int main()
{
    // Initialize window and OpenGL context
    if (!setupGlfwWindow()) {
        return -1;
    }
    
    // Setup ImGui interface
    setupImGui();
    
    // Initialize filters and pipeline
    setupFilterPipeline();
    
    // Main render loop
    while (!glfwWindowShouldClose(mainWindow))
    {
        renderFrame();
    }

    // Cleanup
    cleanupResources();
    
    return 0;
}
