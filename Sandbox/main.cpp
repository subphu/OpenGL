//  Copyright © 2019 Subph. All rights reserved.
//


#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"

const GLuint WIDTH = 600, HEIGHT = 600;
const char* NAME = "Playground";

const std::string windowFailedMessage = "Failed to create GLFW window";
const std::string glewFailedMessage = "Failed to initialize GLEW";

void exitFailure(std::string message) {
    std::cout << message << std::endl;
    glfwTerminate();
    exit(EXIT_FAILURE);
}

void settingGlfw() {
    glfwInit();
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

GLFWwindow* createGLFWwindow(GLuint width, GLuint height, const char* name) {
    GLFWwindow *window = glfwCreateWindow(width, height, name, NULL, NULL);
    if (window == NULL) exitFailure(windowFailedMessage);
    
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    glfwMakeContextCurrent(window);
    glViewport(0, 0, screenWidth, screenHeight);
    return window;
}

void initGlew() {
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) exitFailure(glewFailedMessage);
}

void settingOpenGL() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void processInput(GLFWwindow *window) {
    switch (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
        case GLFW_PRESS:
            glfwSetWindowShouldClose(window, true);
            break;
            
        default:
            break;
    }
}

int main() {
    settingGlfw();
    GLFWwindow *window = createGLFWwindow(WIDTH, HEIGHT, NAME);
    
    initGlew();
    settingOpenGL();
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/shader.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/shader.frag", GL_FRAGMENT_SHADER);
    shader.bindFragDataLocation(0, "fragColor");
    shader.compile();
    
    GLfloat vertices[] = { // x, y, z, r, g, b, a
         0.0f,  0.5f, 0.f, 1.f, 0.f, 0.f, 1.f,
         0.5f, -0.5f, 0.f, 0.f, 1.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.f, 0.f, 0.f, 1.f, 1.f
    };
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    GLint posAttrib = shader.getAttribLocation("position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = shader.getAttribLocation("color");
    glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat) ));
    glEnableVertexAttribArray(colAttrib);
    

    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.setUniform4f("alpha", 1.0f, 1.0f, 1.0f, 1.0f);
        shader.use();

        glBindVertexArray(VAO);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();
    return EXIT_SUCCESS;
}
