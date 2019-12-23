//  Copyright © 2019 Subph. All rights reserved.
//

#include <iostream>

#include "system.h"

void System::pollEvents() {
    glfwPollEvents();
}

void System::settingInput() {
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

bool System::getKeyState(int key) {
    return glfwGetKey(window, key);
}
