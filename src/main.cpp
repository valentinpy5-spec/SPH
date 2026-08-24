
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../engine/include/Camera.h"
#include "../engine/include/Mesh.h"
#include "../engine/include/ShaderProgram.h"
#include "../engine/include/UserInterface.h"

#include "../include/Particle.h"
#include "../include/PerfPanel.h"
#include "../include/SPHSolver.h"
#include "../include/SimulationParameters.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../utils/stb_image_write.h"

#include <cstdio>
#include <eigen3/Eigen/Dense>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

static int g_screenW = 1280, g_screenH = 720;

static void framebufferSizeCallback(GLFWwindow *, int width, int height) {
    glViewport(0, 0, width, height);
    g_screenW = width;
    g_screenH = height;
}

static GLFWwindow *initWindow(int width, int height, const char *title) {
    if (!glfwInit()) {
        std::fprintf(stderr, "glfwInit() failed\n");
        return nullptr;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        std::fprintf(stderr, "glfwCreateWindow() failed\n");
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    glewExperimental = GL_TRUE;
    GLenum glewErr = glewInit();
    if (glewErr != GLEW_OK) {
        std::fprintf(stderr, "glewInit() error: %s\n", glewGetErrorString(glewErr));
    }
    glGetError(); // clear the benign error glewInit() can leave behind

    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, width, height);
    return window;
}

// Ported verbatim from SPH_3D/src/FluidSimulator.cpp — not part of the rendering migration.
std::vector<Particle> LoadParticles(const std::string &path) {
    std::ifstream f(path);
    std::string isboundary;
    int N;
    f >> N;
    std::vector<Particle> particles(N);
    for (auto &p : particles) {
        f >> p.pos[0] >> p.pos[1] >> p.pos[2] >> p.vel[0] >> p.vel[1] >> p.vel[2] >> p.mass >>
            isboundary;
        p.isboundary = (isboundary == "true");
    }
    return particles;
}

void WriteParticles(const char *fname, const std::vector<Particle> &particles) {
    std::ofstream out(fname);
    out << particles.size() << "\n";
    for (const auto &p : particles)
        out << p.pos[0] << " " << p.pos[1] << " " << p.pos[2] << " " << p.vel[0] << " "
            << p.vel[1] << " " << p.vel[2] << "\n";
}

static void savePNG(const char *filename, int width, int height) {
    std::vector<unsigned char> pixels(3 * width * height);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    glReadBuffer(GL_FRONT);
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());
    stbi_flip_vertically_on_write(1);
    stbi_write_png(filename, width, height, 3, pixels.data(), 3 * width);
}

int main(int argc, char **argv) {
    SimulationParameters params = ReadSimulationParameters(argc, argv);
    std::vector<Particle> particles = LoadParticles(params.input_file());
    Eigen::Vector3f gravity = {0.0f, -9.81f, 0.0f};
    SPHSolver solver(particles, params.h(), params.rho0(), params.cs(), params.dt_seconds(),
                      params.nu(), params.xmin(), params.xmax(), params.ymin(), params.ymax(),
                      params.zmin(), params.zmax(), gravity);

    std::size_t n_particles_fluid = 0;
    for (const auto &p : particles)
        if (!p.isboundary) n_particles_fluid++;

    GLFWwindow *window = initWindow(g_screenW, g_screenH, "SPH_ENGINE");
    if (!window) return 1;

    Camera camera(Camera::Mode::FPS);
    camera.orbitCenter = glm::vec3(0.5f, 0.4f, 0.0f); // matches SPH_3D's original camera framing
    camera.radius = 2.5f;
    camera.attachToWindow(window);

    // FPS mode steers via mouse-look, so the cursor must be captured/hidden — matches
    // SPH_3D's original glfwSetInputMode call, otherwise the mouse never drives yaw/pitch and
    // WASD alone feels broken (no way to aim movement).
    if (camera.mode() == Camera::Mode::FPS) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    ShaderProgram instancedShader("shaders/instanced_lit.vert", "shaders/instanced_lit.frag");
    UserInterface ui(window);

    auto sphereMesh = std::make_shared<Mesh>(Mesh::loadOBJ("inputs/lowsphere.obj"));
    sphereMesh->initInstancing(n_particles_fluid);

    PerfPanel perfPanel;
    perfPanel.setParticleCount(n_particles_fluid);
    ui.addPanel(&perfPanel);

    std::vector<glm::vec3> pos(n_particles_fluid);
    const int STEPS_PER_FRAME = 5;
    const float radius = 0.03f / 2.0f;
    int frame = 0;

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        float dt = (float)(now - lastTime);
        lastTime = now;

        glfwPollEvents();
        camera.processKeyboard(window, dt);

        perfPanel.beginPhysics();
        for (int s = 0; s < STEPS_PER_FRAME; s++) {
            solver.BuildHasher();
            solver.UpdateVelocity1();
            solver.ComputeDensity();
            solver.ComputePressure();
            solver.UpdateVelocity2();
            solver.UpdatePosition();
            solver.ApplyBoundaryConditions();
        }
        perfPanel.endPhysics();

        // Eigen -> GLM boundary, copied verbatim from SPH_3D/src/FluidSimulator.cpp.
        std::size_t k = 0;
        for (std::size_t i = 0; i < solver.particles().size(); i++) {
            if (!solver.particles()[i].isboundary) {
                pos[k].x = solver.particles()[i].pos[0];
                pos[k].y = solver.particles()[i].pos[1];
                pos[k].z = solver.particles()[i].pos[2];
                k++;
            }
        }

        perfPanel.beginRender();
        sphereMesh->updateInstancePositions(pos);

        glClearColor(0.8f, 0.8f, 0.8f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float aspect = (float)g_screenW / (float)g_screenH;
        instancedShader.use();
        instancedShader.set("view", camera.getViewMat());
        instancedShader.set("projection", camera.getProjMat(aspect));
        instancedShader.set("radius", radius);
        sphereMesh->drawInstanced(instancedShader, n_particles_fluid);

        ui.beginFrame();
        ui.render();
        perfPanel.endRender();

        glfwSwapBuffers(window);

        if (frame % 30 == 0) {
            char fname[64];
            std::snprintf(fname, sizeof(fname), "frame_%04d.png", frame / 30);
            savePNG(fname, g_screenW, g_screenH);
        }
        frame++;
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
