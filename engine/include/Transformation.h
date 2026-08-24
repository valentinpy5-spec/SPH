#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transformation {
    glm::vec3 position{0.0f};
    glm::vec3 rotation{0.0f}; // Euler angles, radians, applied order Z * Y * X
    glm::vec3 scale{1.0f};

    glm::mat4 getModelMatrix() const {
        glm::mat4 M(1.0f);
        M = glm::translate(M, position);
        M = glm::rotate(M, rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));
        M = glm::rotate(M, rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
        M = glm::rotate(M, rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
        M = glm::scale(M, scale);
        return M;
    }

    static Transformation PositionX(float x) { Transformation t; t.position.x = x; return t; }
    static Transformation PositionY(float y) { Transformation t; t.position.y = y; return t; }
    static Transformation PositionZ(float z) { Transformation t; t.position.z = z; return t; }
    static Transformation Position(float x, float y, float z) {
        Transformation t; t.position = glm::vec3(x, y, z); return t;
    }

    static Transformation RotationX(float angle) { Transformation t; t.rotation.x = angle; return t; }
    static Transformation RotationY(float angle) { Transformation t; t.rotation.y = angle; return t; }
    static Transformation RotationZ(float angle) { Transformation t; t.rotation.z = angle; return t; }

    static Transformation Scale(float sx, float sy, float sz) {
        Transformation t; t.scale = glm::vec3(sx, sy, sz); return t;
    }
    static Transformation Scale(float s) { return Scale(s, s, s); }
};
