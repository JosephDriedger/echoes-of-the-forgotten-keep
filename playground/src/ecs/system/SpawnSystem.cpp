//
// Created by adamd on 2026-03-24.
//

#include "SpawnSystem.h"

#include "ModelManager.h"

std::unordered_map<std::string, SpawnDefinition> SpawnSystem::definitions;

Entity& SpawnSystem::CreateEntity(World& world,
                                  const std::string& name,
                                  const glm::vec3& position,
                                  const glm::vec3& rotation) {

    auto it = definitions.find(name);

    if (it == definitions.end()) {
        throw std::runtime_error("Unknown entity type: " + name);
    }

    const SpawnDefinition& def = it->second;

    auto& entity = world.createEntity();

    entity.addComponent<Transform3D>(position, rotation);

    if (def.model)
        entity.addComponent<Model>(*def.model);

    if (def.texture)
        entity.addComponent<Texture3D>(*def.texture);

    return entity;
}

bool SpawnSystem::RegisterAsset(const std::string& name,
                                const std::string& modelPath,
                                const std::string& texturePath) {

    if (definitions.find(name) != definitions.end()) {
        std::cerr << "SpawnSystem: entity already registered: " << name << std::endl;
        return false;
    }

    SpawnDefinition def(modelPath, texturePath);

    def.model = ModelManager::load(modelPath);

    if (!texturePath.empty()) {
        def.texture = new Texture3D(*TextureManager::load3D(texturePath.c_str()));
    }
    if (def.model != nullptr && def.texture != nullptr) {
        definitions.insert(std::make_pair(name, def));
    }

    std::cout << "Registered: " << name << std::endl;

    return true;
}
