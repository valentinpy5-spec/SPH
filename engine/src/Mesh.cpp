#include "../include/Mesh.h"
#include "../include/ShaderProgram.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>

Mesh::Mesh(std::vector<glm::vec3> vertices, std::vector<glm::vec3> normals,
           std::vector<uint32_t> indices)
    : vertices_(std::move(vertices)), normals_(std::move(normals)), indices_(std::move(indices)) {
    uploadStaticBuffers();
}

void Mesh::uploadStaticBuffers() {
    glGenVertexArrays(1, &vao_);
    glGenBuffers(1, &vboPos_);
    glGenBuffers(1, &vboNorm_);
    glGenBuffers(1, &ebo_);

    glBindVertexArray(vao_);

    glBindBuffer(GL_ARRAY_BUFFER, vboPos_);
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(glm::vec3), vertices_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, vboNorm_);
    glBufferData(GL_ARRAY_BUFFER, normals_.size() * sizeof(glm::vec3), normals_.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(uint32_t), indices_.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Mesh::releaseGL() {
    if (vboInstance_) glDeleteBuffers(1, &vboInstance_);
    if (ebo_) glDeleteBuffers(1, &ebo_);
    if (vboNorm_) glDeleteBuffers(1, &vboNorm_);
    if (vboPos_) glDeleteBuffers(1, &vboPos_);
    if (vao_) glDeleteVertexArrays(1, &vao_);
    vao_ = vboPos_ = vboNorm_ = ebo_ = vboInstance_ = 0;
}

Mesh::~Mesh() { releaseGL(); }

Mesh::Mesh(Mesh &&other) noexcept
    : vao_(other.vao_), vboPos_(other.vboPos_), vboNorm_(other.vboNorm_), ebo_(other.ebo_),
      vboInstance_(other.vboInstance_), vertices_(std::move(other.vertices_)),
      normals_(std::move(other.normals_)), indices_(std::move(other.indices_)) {
    other.vao_ = other.vboPos_ = other.vboNorm_ = other.ebo_ = other.vboInstance_ = 0;
}

Mesh &Mesh::operator=(Mesh &&other) noexcept {
    if (this == &other) return *this;
    releaseGL();
    vao_ = other.vao_;
    vboPos_ = other.vboPos_;
    vboNorm_ = other.vboNorm_;
    ebo_ = other.ebo_;
    vboInstance_ = other.vboInstance_;
    vertices_ = std::move(other.vertices_);
    normals_ = std::move(other.normals_);
    indices_ = std::move(other.indices_);
    other.vao_ = other.vboPos_ = other.vboNorm_ = other.ebo_ = other.vboInstance_ = 0;
    return *this;
}

// Procedural factories use true shared-vertex indexing: unlike the OBJ loader, a
// UV-sphere/plane/cube has no "same position, different normal per face" ambiguity to preserve.

Mesh Mesh::createSphere(float radius, int resolution) {
    std::vector<glm::vec3> vertices, normals;
    std::vector<uint32_t> indices;

    for (int lat = 0; lat <= resolution; lat++) {
        float theta = (float)lat * (float)M_PI / (float)resolution; // 0..pi
        float sinTheta = sin(theta), cosTheta = cos(theta);
        for (int lon = 0; lon <= resolution; lon++) {
            float phi = (float)lon * 2.0f * (float)M_PI / (float)resolution; // 0..2pi
            float sinPhi = sin(phi), cosPhi = cos(phi);

            glm::vec3 n(cosPhi * sinTheta, cosTheta, sinPhi * sinTheta);
            vertices.push_back(radius * n);
            normals.push_back(n);
        }
    }

    int stride = resolution + 1;
    for (int lat = 0; lat < resolution; lat++) {
        for (int lon = 0; lon < resolution; lon++) {
            uint32_t a = lat * stride + lon;
            uint32_t b = a + stride;
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);

            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }

    return Mesh(std::move(vertices), std::move(normals), std::move(indices));
}

Mesh Mesh::createPlane(float width, float height, int subdivX, int subdivY) {
    std::vector<glm::vec3> vertices, normals;
    std::vector<uint32_t> indices;

    for (int y = 0; y <= subdivY; y++) {
        float v = (float)y / (float)subdivY;
        for (int x = 0; x <= subdivX; x++) {
            float u = (float)x / (float)subdivX;
            vertices.emplace_back((u - 0.5f) * width, 0.0f, (v - 0.5f) * height);
            normals.emplace_back(0.0f, 1.0f, 0.0f);
        }
    }

    int stride = subdivX + 1;
    for (int y = 0; y < subdivY; y++) {
        for (int x = 0; x < subdivX; x++) {
            uint32_t a = y * stride + x;
            uint32_t b = a + stride;
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);

            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }

    return Mesh(std::move(vertices), std::move(normals), std::move(indices));
}

Mesh Mesh::createPlaneFromVertices(const std::vector<glm::vec3> &positions, int w, int h) {
    std::vector<glm::vec3> normals(positions.size(), glm::vec3(0.0f, 1.0f, 0.0f));
    std::vector<uint32_t> indices;

    for (int y = 0; y < h - 1; y++) {
        for (int x = 0; x < w - 1; x++) {
            uint32_t a = y * w + x;
            uint32_t b = a + w;
            indices.push_back(a);
            indices.push_back(b);
            indices.push_back(a + 1);

            indices.push_back(a + 1);
            indices.push_back(b);
            indices.push_back(b + 1);
        }
    }

    Mesh mesh(positions, std::move(normals), std::move(indices));
    mesh.updateNormals(); // positions came from the caller, not a flat plane: recompute normals
    return mesh;
}

Mesh Mesh::createCube(float size) {
    float h = size * 0.5f;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<uint32_t> indices;

    struct Face { glm::vec3 n; glm::vec3 u, v; };
    const Face faces[6] = {
        {{0, 0, 1}, {1, 0, 0}, {0, 1, 0}},  // front
        {{0, 0, -1}, {-1, 0, 0}, {0, 1, 0}}, // back
        {{-1, 0, 0}, {0, 0, 1}, {0, 1, 0}}, // left
        {{1, 0, 0}, {0, 0, -1}, {0, 1, 0}}, // right
        {{0, 1, 0}, {1, 0, 0}, {0, 0, -1}}, // top
        {{0, -1, 0}, {1, 0, 0}, {0, 0, 1}}, // bottom
    };

    for (const Face &f : faces) {
        uint32_t base = (uint32_t)vertices.size();
        glm::vec3 center = f.n * h;
        vertices.push_back(center - h * f.u - h * f.v);
        vertices.push_back(center + h * f.u - h * f.v);
        vertices.push_back(center + h * f.u + h * f.v);
        vertices.push_back(center - h * f.u + h * f.v);
        for (int i = 0; i < 4; i++) normals.push_back(f.n);

        indices.push_back(base + 0);
        indices.push_back(base + 1);
        indices.push_back(base + 2);
        indices.push_back(base + 0);
        indices.push_back(base + 2);
        indices.push_back(base + 3);
    }

    return Mesh(std::move(vertices), std::move(normals), std::move(indices));
}

// Flattened-per-face (indices = 0,1,2,3,...): a deliberate simplification, not an oversight.

Mesh Mesh::loadOBJ(const std::string &path) {
    std::vector<glm::vec3> inputPoints, inputNormals;
    std::vector<glm::uvec3> triangles, normalIndices;

    std::ifstream objStream(path);
    if (!objStream.is_open()) {
        std::cerr << "Mesh::loadOBJ: could not open " << path << "\n";
        return Mesh({}, {}, {});
    }

    std::string line;
    while (std::getline(objStream, line)) {
        std::stringstream ss(line);
        std::string label;
        ss >> label;

        if (label == "v") {
            glm::vec3 p;
            ss >> p.x >> p.y >> p.z;
            inputPoints.push_back(p);
        } else if (label == "vn") {
            glm::vec3 n;
            ss >> n.x >> n.y >> n.z;
            inputNormals.push_back(n);
        } else if (label == "f") {
            glm::uvec3 tri, normIdx;
            for (int i = 0; i < 3; i++) {
                std::string s;
                ss >> s;
                size_t pos = s.find('/');
                std::string v = s.substr(0, pos);
                tri[i] = (uint32_t)std::stoi(v) - 1;
                s.erase(0, pos + 1);
                pos = s.find('/');
                s.erase(0, pos + 1);
                normIdx[i] = (uint32_t)std::stoi(s) - 1;
            }
            triangles.push_back(tri);
            normalIndices.push_back(normIdx);
        }
    }

    std::vector<glm::vec3> vertices, normals;
    std::vector<uint32_t> indices;
    vertices.reserve(triangles.size() * 3);
    normals.reserve(triangles.size() * 3);
    indices.reserve(triangles.size() * 3);

    for (size_t i = 0; i < triangles.size(); i++) {
        vertices.push_back(inputPoints[triangles[i].x]);
        vertices.push_back(inputPoints[triangles[i].y]);
        vertices.push_back(inputPoints[triangles[i].z]);
        normals.push_back(inputNormals[normalIndices[i].x]);
        normals.push_back(inputNormals[normalIndices[i].y]);
        normals.push_back(inputNormals[normalIndices[i].z]);
        indices.push_back((uint32_t)(i * 3 + 0));
        indices.push_back((uint32_t)(i * 3 + 1));
        indices.push_back((uint32_t)(i * 3 + 2));
    }

    return Mesh(std::move(vertices), std::move(normals), std::move(indices));
}

void Mesh::draw(const ShaderProgram &shader, const glm::mat4 &model) const {
    shader.use();
    shader.set("model", model);
    glBindVertexArray(vao_);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices_.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::startDrawMultiple(const ShaderProgram &shader) const {
    shader.use();
    glBindVertexArray(vao_);
}

void Mesh::addDrawMultiple(const ShaderProgram &shader, const glm::mat4 &model) const {
    shader.set("model", model);
    glDrawElements(GL_TRIANGLES, (GLsizei)indices_.size(), GL_UNSIGNED_INT, 0);
}

void Mesh::endDrawMultiple() const { glBindVertexArray(0); }

void Mesh::initInstancing(size_t maxInstances) {
    glBindVertexArray(vao_);

    glGenBuffers(1, &vboInstance_);
    glBindBuffer(GL_ARRAY_BUFFER, vboInstance_);
    glBufferData(GL_ARRAY_BUFFER, maxInstances * sizeof(glm::vec3), nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void *)0);
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    glBindVertexArray(0);
}

void Mesh::updateInstancePositions(const std::vector<glm::vec3> &positions) {
    glBindBuffer(GL_ARRAY_BUFFER, vboInstance_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(glm::vec3), positions.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::drawInstanced(const ShaderProgram &shader, size_t count) const {
    shader.use();
    glBindVertexArray(vao_);
    glDrawElementsInstanced(GL_TRIANGLES, (GLsizei)indices_.size(), GL_UNSIGNED_INT, 0, (GLsizei)count);
    glBindVertexArray(0);
}

void Mesh::updateVertices(const std::vector<glm::vec3> &positions) {
    vertices_ = positions;
    glBindBuffer(GL_ARRAY_BUFFER, vboPos_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_.size() * sizeof(glm::vec3), vertices_.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::updateNormals() {
    std::vector<glm::vec3> accum(vertices_.size(), glm::vec3(0.0f));

    for (size_t i = 0; i + 2 < indices_.size(); i += 3) {
        uint32_t ia = indices_[i], ib = indices_[i + 1], ic = indices_[i + 2];
        glm::vec3 faceNormal = glm::cross(vertices_[ib] - vertices_[ia], vertices_[ic] - vertices_[ia]);
        accum[ia] += faceNormal;
        accum[ib] += faceNormal;
        accum[ic] += faceNormal;
    }

    for (auto &n : accum) {
        float len = glm::length(n);
        n = (len > 1e-8f) ? (n / len) : glm::vec3(0.0f, 1.0f, 0.0f);
    }
    normals_ = accum;

    glBindBuffer(GL_ARRAY_BUFFER, vboNorm_);
    glBufferSubData(GL_ARRAY_BUFFER, 0, normals_.size() * sizeof(glm::vec3), normals_.data());
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
