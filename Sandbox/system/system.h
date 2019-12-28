//  Copyright © 2019 Subph. All rights reserved.
//

#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#define KEY_RELEASE GLFW_RELEASE    // 0
#define KEY_PRESS GLFW_PRESS        // 1
#define KEY_PRESSED GLFW_REPEAT     // 2

enum keyState {
    key_release, key_press, key_pressed
};

enum key {
    key_esc = GLFW_KEY_ESCAPE,
    key_w = GLFW_KEY_W,
    key_a = GLFW_KEY_A,
    key_s = GLFW_KEY_S,
    key_d = GLFW_KEY_D,
    key_q = GLFW_KEY_Q,
    key_e = GLFW_KEY_E
};

template<typename T> struct Size {
    T width, height;
};

class System {
    
public:
    glm::vec2 cursorPos;
    
    void initGlLibrary();
    void settingInput();
    void enableCursor();
    void disableCursor();
    
    void settingWindow();
    void createWindow(GLuint width, GLuint height, const char* name);
    
    void closeWindow();
    void exitFailure(std::string message);
    
    void swapBuffer();
    void pollEvents();
    
    bool getWindowState();
    Size<int> getWindowSize();
    
    bool getKeyState(int key);
    glm::vec2 getCursorPosition();
    glm::vec2 getCursorMovement();
    
    void setScrollCallback(void (*callback)(float, float));
    
    float getTime();

    static System& instance() {
        static System instance; // Guaranteed to be destroyed. Instantiated on first use.
        return instance;
    }
    
private:
    GLFWwindow* window;
    void (*scrollCallback)(float, float);
    
    static void glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    
    System();
    ~System();

    // C++ 03
    // ========
    // Don't forget to declare these two. You want to make sure they
    // are unacceptable otherwise you may accidentally get copies of
    // your singleton appearing.
    System(System const&);         // Don't Implement
    void operator=(System const&); // Don't implement
    
};

