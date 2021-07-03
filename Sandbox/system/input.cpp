//  Copyright © 2019 Subph. All rights reserved.
//

#include "system.h"

void System::pollEvents() {
    glfwPollEvents();
}

void System::settingInput() {
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);
}

void System::enableCursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void System::disableCursor() {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void System::setScrollCallback(void (*callback)(float, float)) {
    scrollCallback = callback;
    glfwSetScrollCallback(window, glfwScrollCallback);
}

void System::setMouseCallback(void (*callback)(int, int)) {
    mouseCallback = callback;
    glfwSetMouseButtonCallback(window, glfwMouseCallback);
}

void System::glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    System::instance().scrollCallback(xoffset, yoffset);
}

void System::glfwMouseCallback(GLFWwindow* window, int button, int action, int mods) {
    System::instance().mouseCallback(button, action);
}

bool System::getKeyState(int key) {
    return glfwGetKey(window, key);
}

glm::vec2 System::updateCursorPosition() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    cursorPos = glm::vec2(x, -y);
    return cursorPos;
}

glm::vec2 System::getCursorMovement() {
    if (cursorPos.x == 0 && cursorPos.y == 0) {
        updateCursorPosition();
        return glm::vec2(0, 0);
    }
    glm::vec2 lastPos = cursorPos;
    updateCursorPosition();
    glm::vec2 move = glm::vec2(cursorPos.x - lastPos.x, cursorPos.y - lastPos.y);
    return glm::clamp(move, glm::vec2(-99, -99), glm::vec2(99, 99));
}
