// Compile/link a vertex+fragment shader pair and expose typed uniform setters.
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <string>

class ShaderProgram {
public:
    ShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);
    ~ShaderProgram();

    ShaderProgram(const ShaderProgram &) = delete;
    ShaderProgram &operator=(const ShaderProgram &) = delete;

    void use() const;
    GLuint id() const { return program_; }

    void set(const char *name, int value) const;
    void set(const char *name, float value) const;
    void set(const char *name, const glm::vec3 &value) const;
    void set(const char *name, const glm::mat4 &value) const;

private:
    GLuint program_ = 0;

    static GLuint compile(const std::string &path, GLenum type);
    static GLuint link(GLuint vertexShader, GLuint fragmentShader);
};
