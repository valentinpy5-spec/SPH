#include "../include/Camera.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>

// Single active camera, set by attachToWindow(). File-local (not exposed in the header) so
// Camera stays self-contained without forcing every demo's main.cpp to declare a global.
static Camera *g_activeCamera = nullptr;

Camera::Camera(Mode mode) : mode_(mode) {}

void Camera::attachToWindow(GLFWwindow *window) {
    g_activeCamera = this;
    glfwSetCursorPosCallback(window, &Camera::cursorPosCallback);
    glfwSetScrollCallback(window, &Camera::scrollCallback);
    glfwSetMouseButtonCallback(window, &Camera::mouseButtonCallback);
}

glm::vec3 Camera::frontFromEuler() const {
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(dir);
}

glm::vec3 Camera::orbitPosition() const {
    glm::vec3 dir;
    dir.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return orbitCenter + radius * dir;
}

glm::mat4 Camera::getViewMat() const {
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    if (mode_ == Mode::FPS) {
        return glm::lookAt(position, position + frontFromEuler(), up);
    } else {
        return glm::lookAt(orbitPosition(), orbitCenter, up);
    }
}

glm::mat4 Camera::getProjMat(float aspect) const {
    return glm::perspective(fov, aspect, near, far);
}

glm::vec3 Camera::getPosition() const {
    return (mode_ == Mode::FPS) ? position : orbitPosition();
}

glm::vec3 Camera::getRay(double xpos, double ypos, int screenW, int screenH, float aspect) const {
    const glm::vec3 up(0.0f, 1.0f, 0.0f);
    glm::vec3 forward = (mode_ == Mode::FPS) ? frontFromEuler()
                                              : glm::normalize(orbitCenter - orbitPosition());
    glm::vec3 right = glm::normalize(glm::cross(forward, up));
    glm::vec3 camUp = glm::cross(right, forward);

    float viewportHeight = 2.0f * tan(fov / 2.0f);
    float viewportWidth = viewportHeight * aspect;

    float u = (float)xpos / (float)screenW;
    float v = (float)ypos / (float)screenH;
    float px = (2.0f * u - 1.0f) * viewportWidth / 2.0f;
    float py = (1.0f - 2.0f * v) * viewportHeight / 2.0f;

    return glm::normalize(forward + px * right + py * camUp);
}

void Camera::processKeyboard(GLFWwindow *window, float dt) {
    if (mode_ != Mode::FPS) return;
    float speed = moveSpeed * dt;
    glm::vec3 front = frontFromEuler();
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0.0f, 1.0f, 0.0f)));

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) position += speed * front;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) position -= speed * front;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) position -= speed * right;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) position += speed * right;
}

void Camera::cursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
    if (!g_activeCamera) return;
    Camera &cam = *g_activeCamera;

    if (cam.mode_ == Mode::FPS) {
        if (cam.firstMouse_) {
            cam.lastX_ = xpos;
            cam.lastY_ = ypos;
            cam.firstMouse_ = false;
        }
        float xoffset = (float)(xpos - cam.lastX_);
        float yoffset = (float)(cam.lastY_ - ypos);
        cam.lastX_ = xpos;
        cam.lastY_ = ypos;

        const float sensitivity = 0.1f;
        cam.yaw += xoffset * sensitivity;
        cam.pitch += yoffset * sensitivity;
    } else {
        if (!cam.rotating_) return;
        if (cam.firstMouse_) {
            cam.lastX_ = xpos;
            cam.lastY_ = ypos;
            cam.firstMouse_ = false;
        }
        float xoffset = (float)(xpos - cam.lastX_);
        float yoffset = (float)(cam.lastY_ - ypos);
        cam.lastX_ = xpos;
        cam.lastY_ = ypos;

        const float sensitivity = 0.1f;
        cam.yaw += xoffset * sensitivity;
        cam.pitch += yoffset * sensitivity;
    }

    if (cam.pitch > 89.0f) cam.pitch = 89.0f;
    if (cam.pitch < -89.0f) cam.pitch = -89.0f;
}

void Camera::scrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
    if (!g_activeCamera) return;
    Camera &cam = *g_activeCamera;

    if (cam.mode_ == Mode::FPS) {
        cam.fov -= glm::radians((float)yoffset);
        if (cam.fov < glm::radians(1.0f)) cam.fov = glm::radians(1.0f);
        if (cam.fov > glm::radians(45.0f)) cam.fov = glm::radians(45.0f);
    } else {
        cam.radius -= (float)yoffset * 0.2f;
        if (cam.radius < 0.1f) cam.radius = 0.1f;
    }
}

void Camera::mouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (!g_activeCamera) return;
    Camera &cam = *g_activeCamera;
    if (cam.mode_ != Mode::Orbit) return;

    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            cam.rotating_ = true;
            cam.firstMouse_ = true;
        } else if (action == GLFW_RELEASE) {
            cam.rotating_ = false;
        }
    }
}
