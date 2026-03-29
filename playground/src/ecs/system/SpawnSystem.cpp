//
// Created by adamd on 2026-03-24.
//

#include "SpawnSystem.h"

#include "ModelManager.h"

std::unordered_map<AssetType, SpawnDefinition, EnumClassHash> SpawnSystem::definitions;

Entity& SpawnSystem::CreateEntity(World& world,
                                  AssetType type,
                                  const glm::vec3& position,
                                  const glm::vec3& rotation) {

    auto it = definitions.find(type);

    if (it == definitions.end()) {
        throw std::runtime_error("Unknown asset type");
    }

    const SpawnDefinition& def = it->second;

    auto& entity = world.createEntity();

    glm::vec3 finalPos = position + def.offset;

    entity.addComponent<Transform3D>(finalPos, rotation, def.scale);

    if (def.model)
        entity.addComponent<Model>(*def.model);

    if (def.textureID != 0)
        entity.addComponent<Texture3D>(def.textureID);

    if (def.hasCollider)
        entity.addComponent<Collider3D>();

    if (def.customSetup)
        def.customSetup(entity);

    return entity;
}

bool SpawnSystem::RegisterAsset(AssetType type,
                                const std::string& modelPath,
                                const std::string& texturePath) {

    if (definitions.contains(type)) {
        std::cerr << "Already registered\n";
        return false;
    }

    SpawnDefinition def;

    def.model = ModelManager::load(modelPath);

    if (!texturePath.empty()) {
        GLuint* tex = TextureManager::load3D(texturePath);
        if (tex) def.textureID = *tex;
    }

    if (!def.model) {
        std::cerr << "Failed to load model\n";
        return false;
    }

    definitions[type] = def;

    return true;
}
