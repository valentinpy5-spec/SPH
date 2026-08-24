#pragma once

#include <GL/glew.h>
#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class ShaderProgram;

class Mesh {
public:
    Mesh(std::vector<glm::vec3> vertices,
         std::vector<glm::vec3> normals,
         std::vector<uint32_t> indices);

    ~Mesh();
    Mesh(const Mesh&) = delete;
    Mesh &operator=(const Mesh&) = delete;
    Mesh(Mesh&& other) noexcept;
    Mesh &operator=(Mesh&& other) noexcept;


    static Mesh createSphere(float radius = 1.0f, int resolution = 24);
    static Mesh createPlane(float width = 1.0f, float height = 1.0f, int subdivX = 1, int subdivY = 1);
    static Mesh createCube(float size = 1.0f);

    static Mesh createPlaneFromVertices(const std::vector<glm::vec3> &positions, int w, int h);


    static Mesh loadOBJ(const std::string &path);


    void draw(const ShaderProgram &shader, const glm::mat4 &model) const;


    void startDrawMultiple(const ShaderProgram &shader) const;
    void addDrawMultiple(const ShaderProgram &shader, const glm::mat4 &model) const;
    void endDrawMultiple() const;

    void initInstancing(size_t maxInstances);
    void updateInstancePositions(const std::vector<glm::vec3> &positions);
    void drawInstanced(const ShaderProgram &shader, size_t count) const;


    void updateVertices(const std::vector<glm::vec3> &positions);
    void updateNormals(); 

    GLuint vao() const { return vao_; }
    size_t indexCount() const { return indices_.size(); }
    const std::vector<glm::vec3> &vertices() const { return vertices_; }
    const std::vector<glm::vec3> &normals() const { return normals_; }
    const std::vector<uint32_t> &indices() const { return indices_; }

private:
    GLuint vao_ = 0, vboPos_ = 0, vboNorm_ = 0, ebo_ = 0, vboInstance_ = 0;
    std::vector<glm::vec3> vertices_, normals_;
    std::vector<uint32_t> indices_;

    void uploadStaticBuffers(); 
    void releaseGL();
};
