//
// Created by adamd on 2026-03-24.
//

#include "SpawnSystem.h"

#include "ModelManager.h"

std::unordered_map<std::string, SpawnDefinition*> SpawnSystem::definitions;

Entity& SpawnSystem::CreateEntity(World& world,
                            const std::string& name,
                            const glm::vec3& position,
                            const glm::vec3& rotation) {

    auto it = definitions.find(name);

    if (it == definitions.end()) {
        std::cerr << "SpawnSystem: Unknown entity type: " << name << std::endl;
    }

    const SpawnDefinition* def = it->second;

    auto& entity = world.createEntity();

    // Transform
    entity.addComponent<Transform3D>(position, rotation);

    // Model
    if (!def->modelPath.empty()) {
        entity.addComponent<Model>(*ModelManager::load(def->modelPath));
    }

    // Texture
    if (def->texturePath) {
        entity.addComponent<Texture3D>(*TextureManager::load3D(def->texturePath));
    }

    // Collider
    //entity.addComponent<Collider3D>();

    return entity;
}

bool SpawnSystem::RegisterAsset(const std::string name, const std::string &modelPath, const char *texturePath) {
    auto it = definitions.find(name);

    if (it != definitions.end()) {
        std::cerr << "SpawnSystem: entity already registered: " << name << std::endl;
        return false;
    }
    auto* definition = new SpawnDefinition(modelPath, texturePath);

    return definitions[name] = definition;
}
