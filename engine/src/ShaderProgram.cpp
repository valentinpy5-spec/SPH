#include "../include/ShaderProgram.h"

#include <fstream>
#include <iostream>
#include <sstream>

GLuint ShaderProgram::compile(const std::string &path, GLenum type) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "ShaderProgram: could not open " << path << "\n";
        return 0;
    }
    std::stringstream ss;
    ss << file.rdbuf();
    std::string source = ss.str();

    GLuint shader = glCreateShader(type);
    const GLchar *sourceC = source.c_str();
    GLint sourceLen = (GLint)source.length();
    glShaderSource(shader, 1, &sourceC, &sourceLen);
    glCompileShader(shader);

    GLint ok = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
        std::cerr << "ShaderProgram: compile error (" << path << "):\n" << log << "\n";
        glDeleteShader(shader);
        return 0;
    }
    return shader;
}

GLuint ShaderProgram::link(GLuint vertexShader, GLuint fragmentShader) {
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vertexShader);
    glAttachShader(prog, fragmentShader);
    glLinkProgram(prog);

    GLint ok = 0;
    glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        char log[1024];
        glGetProgramInfoLog(prog, sizeof(log), nullptr, log);
        std::cerr << "ShaderProgram: link error:\n" << log << "\n";
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return prog;
}

ShaderProgram::ShaderProgram(const std::string &vertexPath, const std::string &fragmentPath) {
    GLuint vs = compile(vertexPath, GL_VERTEX_SHADER);
    GLuint fs = compile(fragmentPath, GL_FRAGMENT_SHADER);
    program_ = link(vs, fs);
}

ShaderProgram::~ShaderProgram() {
    if (program_) glDeleteProgram(program_);
}

void ShaderProgram::use() const { glUseProgram(program_); }

void ShaderProgram::set(const char *name, int value) const {
    glUniform1i(glGetUniformLocation(program_, name), value);
}
void ShaderProgram::set(const char *name, float value) const {
    glUniform1f(glGetUniformLocation(program_, name), value);
}
void ShaderProgram::set(const char *name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(program_, name), 1, &value[0]);
}
void ShaderProgram::set(const char *name, const glm::mat4 &value) const {
    glUniformMatrix4fv(glGetUniformLocation(program_, name), 1, GL_FALSE, &value[0][0]);
}
