//  Copyright © 2019 Subph. All rights reserved.
//

#include "system.h"
#include "define.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define WINDOW_FAILED_MESSAGE "Failed to create GLFW window"
#define GLEW_FAILED_MESSAGE   "Failed to initialize GLEW"
#define EXIT_MESSAGE          "Window closed"

void System::initGlLibrary() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) exitFailure(GLEW_FAILED_MESSAGE);
}

void System::settingWindow() {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void System::createWindow() {
    window = glfwCreateWindow(settings->windowSize().width,
                              settings->windowSize().height,
                              settings->name, NULL, NULL);
    if (window == NULL) exitFailure(WINDOW_FAILED_MESSAGE);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, screenWidth, screenHeight);
    settings->setBufferSize({ screenWidth, screenHeight });
}

void System::createWindow(GLuint width, GLuint height, const char* name) {
    settings = new Settings();
    settings->name = name;
    settings->setWindowSize({ (int)width, (int)height });
    createWindow();
}

void System::closeWindow() {
    std::cout << EXIT_MESSAGE << std::endl;
    glfwSetWindowShouldClose(window, true);
}

void System::exitFailure(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
}

void System::settingUI() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
//    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
//
//    Setup Dear ImGui style
//    ImGui::StyleColorsDark();
    ImGui::StyleColorsClassic();

//    Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(GLSL_VERSION);
}

bool System::getWindowState() {
    return !glfwWindowShouldClose(window);
}

Size<int> System::getWindowSize() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    settings->setWindowSize({ width, height });
    return settings->windowSize();
}

Size<int> System::getFramebufferSize() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    settings->setBufferSize({ width, height });
    return settings->bufferSize();
}

void System::swapBuffer() {
    glfwSwapBuffers(window);
}
