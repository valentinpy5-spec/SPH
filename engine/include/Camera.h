#pragma once

#include <glm/glm.hpp>

struct GLFWwindow;

class Camera {
public:
    enum class Mode { FPS, Orbit };

    explicit Camera(Mode mode = Mode::Orbit);

    void attachToWindow(GLFWwindow *window);


    void processKeyboard(GLFWwindow *window, float dt);

    glm::mat4 getViewMat() const;
    glm::mat4 getProjMat(float aspect) const;
    glm::vec3 getPosition() const;


    glm::vec3 getRay(double xpos, double ypos, int screenW, int screenH, float aspect) const;

    void setMode(Mode m) { mode_ = m; }
    Mode mode() const { return mode_; }


    glm::vec3 orbitCenter = glm::vec3(0.0f);
    float radius = 3.0f;


    float yaw = -90.0f;
    float pitch = 0.0f;
    float fov = glm::radians(45.0f);
    float near = 0.01f;
    float far = 100.0f;


    glm::vec3 position = glm::vec3(0.0f, 1.0f, 3.0f);
    float moveSpeed = 2.5f;

private:
    Mode mode_;
    bool firstMouse_ = true;
    bool rotating_ = false; // orbit mode: only rotate while a mouse button is held
    double lastX_ = 0.0, lastY_ = 0.0;

    glm::vec3 frontFromEuler() const; // FPS mode: look direction from yaw/pitch
    glm::vec3 orbitPosition() const;  // Orbit mode: cameraPos from orbitCenter+radius+yaw/pitch

    static void cursorPosCallback(GLFWwindow *window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow *window, double xoffset, double yoffset);
    static void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
};
