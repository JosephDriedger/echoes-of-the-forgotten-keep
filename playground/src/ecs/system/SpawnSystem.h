//
// Created by adamd on 2026-03-24.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H
#include "Entity.h"
#include "World.h"

struct SpawnDefinition {
    const std::string& modelPath;
    const char* texturePath;


    // Future expansion
    // const Collider3D& collider3D;
    // bool hasAnimator = false;
    // AnimationClips, sounds, scripts, etc.
};

class SpawnSystem {
public:
    static Entity& CreateEntity(World& world,
                  const std::string& name,
                  const glm::vec3& position,
                  const glm::vec3& rotation = {0,0,0});
    static bool RegisterAsset(const std::string name,const std::string& modelPath, const char* texturePath);

private:
    static std::unordered_map<std::string, SpawnDefinition*> definitions;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H