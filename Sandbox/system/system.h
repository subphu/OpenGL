//  Copyright © 2019 Subph. All rights reserved.
//

#pragma once

#include <GLFW/glfw3.h>
#include <iostream>

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
    key_d = GLFW_KEY_D
};

template<typename T> struct Size {
    T width, height;
};

class System {
    
public:
    
    void initGlLibrary();
    void settingOpenGL();
    void settingInput();
    
    void settingWindow();
    void createWindow(GLuint width, GLuint height, const char* name);
    
    void closeWindow();
    void exitFailure(std::string message);
    
    void swapBuffer();
    void pollEvents();
    
    bool getWindowState();
    Size<int> getWindowSize();
    
    bool getKeyState(int key);
    
    float getTime();

    static System& instance() {
        static System instance; // Guaranteed to be destroyed. Instantiated on first use.
        return instance;
    }
    
private:
    GLFWwindow* window;
    
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

