//  Copyright © 2019 Subph. All rights reserved.
//

#define GLEW_STATIC

#include <GL/glew.h>
#include <iostream>

#include "system.h"

#define WINDOW_FAILED_MESSAGE "Failed to create GLFW window"
#define GLEW_FAILED_MESSAGE   "Failed to initialize GLEW"
#define EXIT_MESSAGE          "Window closed"

void System::initGlLibrary() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) exitFailure(GLEW_FAILED_MESSAGE);
}

void System::settingOpenGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void System::settingWindow() {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

void System::createWindow(GLuint width, GLuint height, const char* name) {
    window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (window == NULL) exitFailure(WINDOW_FAILED_MESSAGE);

    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, screenWidth, screenHeight);
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

bool System::getWindowState() {
    return !glfwWindowShouldClose(window);
}

Size<int> System::getWindowSize() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    return {width, height};
}

void System::swapBuffer() {
    glfwSwapBuffers(window);
}
