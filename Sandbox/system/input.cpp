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

void System::glfwScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    System::instance().scrollCallback(xoffset, yoffset);
}

bool System::getKeyState(int key) {
    return glfwGetKey(window, key);
}

glm::vec2 System::getCursorPosition() {
    double x, y;
    glfwGetCursorPos(window, &x, &y);
    cursorPos = glm::vec2(x, -y);
    return cursorPos;
}

glm::vec2 System::getCursorMovement() {
    if (cursorPos.x == 0 && cursorPos.y == 0) {
        getCursorPosition();
        return glm::vec2(0, 0);
    }
    glm::vec2 lastPos = cursorPos;
    getCursorPosition();
    return glm::vec2(cursorPos.x - lastPos.x, cursorPos.y - lastPos.y);
}
