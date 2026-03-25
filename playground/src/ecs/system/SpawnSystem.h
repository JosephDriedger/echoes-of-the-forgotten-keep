//
// Created by adamd on 2026-03-24.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H
#include "Entity.h"
#include "World.h"

struct SpawnDefinition {
    std::string modelPath;
    std::string texturePath;

    Model* model = nullptr;
    Texture3D* texture = nullptr;

    SpawnDefinition(const std::string& m, const std::string& t)
        : modelPath(m), texturePath(t) {}
};

class SpawnSystem {
public:
    static std::unordered_map<std::string, SpawnDefinition> definitions;
    static Entity& CreateEntity(World& world,
                  const std::string& name,
                  const glm::vec3& position,
                  const glm::vec3& rotation = {0,0,0});
    static bool RegisterAsset(const std::string& name,const std::string& modelPath, const std::string& texturePath);

};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H