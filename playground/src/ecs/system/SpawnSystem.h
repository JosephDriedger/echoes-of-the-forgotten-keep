//
// Created by adamd on 2026-03-24.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H
#include "Entity.h"
#include "World.h"

enum class AssetType {
    Player,
    Enemy,
    Wall,
    WallCorner,
    WallCrossing,
    WallTsplit,
    WallDoorwayScaffold,
    Door,
    FloorTileLarge,
    FloorTileSmall,

};

struct EnumClassHash {
    template <typename T>
    std::size_t operator()(T t) const {
        return static_cast<std::size_t>(t);
    }
};

struct SpawnDefinition {

    Model* model = nullptr;
    GLuint textureID = 0;

    glm::vec3 scale = {1,1,1};
    glm::vec3 offset = {0,0,0};

    bool hasCollider = false;

    std::function<void(Entity&)> customSetup;
};

class SpawnSystem {
public:
    static std::unordered_map<AssetType, SpawnDefinition, EnumClassHash> definitions;
    static Entity& CreateEntity(World& world,
                  AssetType type,
                  const glm::vec3& position,
                  const glm::vec3& rotation = {0,0,0});
    static bool RegisterAsset(AssetType type,const std::string& modelPath, const std::string& texturePath);

};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_SPAWNSYSTEM_H