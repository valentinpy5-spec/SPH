#pragma once

#include "Mesh.h"
#include "Transformation.h"

#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>

class ShaderProgram;

struct SceneObject {
    std::string name;
    std::shared_ptr<Mesh> mesh;
    Transformation transform;
    glm::vec3 color{0.8f, 0.8f, 0.8f}; 
    bool visible = true;
};

class ObjectsManager {
public:
    SceneObject &add(const std::string &name, std::shared_ptr<Mesh> mesh,
                      Transformation transform = {});
    void remove(const std::string &name);
    SceneObject *find(const std::string &name); // nullptr if absent

    std::vector<SceneObject> &objects() { return objects_; }
    const std::vector<SceneObject> &objects() const { return objects_; }

    void drawAll(const ShaderProgram &shader, const glm::mat4 &view, const glm::mat4 &proj) const;

private:
    std::vector<SceneObject> objects_;
};
