#include "../include/ObjectsManager.h"
#include "../include/ShaderProgram.h"

#include <algorithm>

SceneObject &ObjectsManager::add(const std::string &name, std::shared_ptr<Mesh> mesh,
                                  Transformation transform) {
    objects_.push_back(SceneObject{name, std::move(mesh), transform});
    return objects_.back();
}

void ObjectsManager::remove(const std::string &name) {
    objects_.erase(std::remove_if(objects_.begin(), objects_.end(),
                                   [&](const SceneObject &o) { return o.name == name; }),
                   objects_.end());
}

SceneObject *ObjectsManager::find(const std::string &name) {
    for (auto &o : objects_)
        if (o.name == name) return &o;
    return nullptr;
}

void ObjectsManager::drawAll(const ShaderProgram &shader, const glm::mat4 &view,
                              const glm::mat4 &proj) const {
    shader.use();
    shader.set("view", view);
    shader.set("projection", proj);

    for (const auto &obj : objects_) {
        if (!obj.visible || !obj.mesh) continue;
        shader.set("objectColor", obj.color);
        obj.mesh->draw(shader, obj.transform.getModelMatrix());
    }
}
