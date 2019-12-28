//  Copyright © 2019 Subph. All rights reserved.
//

#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#include "camera.h"
#include "define.h"
#include "system/system.h"

#define FRONT    glm::vec3(0.0f, 0.0f, 0.0f)
#define WORLD_UP glm::vec3(0.0f, 1.0f, 0.0f)
#define POSITION glm::vec3(0.0f, 0.0f, 5.0f)

#define MAX_ZOOM  45.0f
#define MAX_PITCH 89.0f
#define SPEED     0.10f
#define SENSITIVITY   0.1f
#define VIEW_DISTANCE 45.0f

#define YAW   0.0f
#define PITCH 0.0f
#define ROLL  0.0f

Camera::Camera(glm::vec3 position, glm::vec3 direction) {
    reset();
    setPosition(position);
    lookAt(direction);
    updateVector();
}

void Camera::reset() {
    axis = 1;
    lockFocus = false;
    useConstraint = true;
    maxZoom = MAX_ZOOM;
    maxPitch = MAX_PITCH;
    
    front = FRONT;
    worldUp = WORLD_UP;
    position = POSITION;
    speed = SPEED;
    sensitivity = SENSITIVITY;
    viewDistance = VIEW_DISTANCE;
    
    yaw = YAW;
    pitch = PITCH;
    roll = ROLL;
}

glm::mat4 Camera::getViewMatrix() {
    return glm::lookAt(position, position + front, up);
}

glm::mat4 Camera::getProjection(float ratio) {
    return glm::perspective(glm::radians(viewDistance), ratio, 0.1f, 100.0f);
}

void Camera::setInvertedAxis(bool value) {
    axis = value ? -1 : 1;
}

void Camera::setLockFocus(bool value) {
    lockFocus = value;
}

void Camera::setPosition(glm::vec3 pos) {
    position = pos;
}

void Camera::lookAt(glm::vec3 focusPos) {
    focusPoint = focusPos;
    front = glm::normalize(focusPos - position);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
    updateRotation();
}

void Camera::zoom(float delta) {
    viewDistance += delta;
    viewDistance = fmax(fmin(viewDistance, maxZoom), 1.0f);
}

void Camera::move(glm::vec3 direction) {
    glm::vec3 velocity = direction * speed;
    position += front * velocity.z;
    position += right * velocity.x;
    position += up    * velocity.y;
    
    if (lockFocus) lookAt(focusPoint);
}

void Camera::turn(glm::vec2 delta) {
    if (lockFocus) return;
    
    yaw   += axis * delta.x * sensitivity;
    pitch += axis * delta.y * sensitivity;
    pitch = useConstraint ? fmax(fmin(pitch, maxPitch), -maxPitch) : pitch;
    updateVector();
}

void Camera::updateVector() {
    glm::vec3 newFront;
    newFront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    newFront.y = sin(glm::radians(pitch));
    newFront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    
    front = glm::normalize(newFront);
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}

void Camera::updateRotation() {
    yaw = glm::degrees(atan2(front.z, front.x));
    pitch = glm::degrees(atan2(front.y, -front.z));
}