//
// Created by adamd on 2026-02-25.
//

#ifndef ASSIGNMENT1_SCENE_H
#define ASSIGNMENT1_SCENE_H
#include <SDL3/SDL_events.h>
#include "World.h"
#include "SceneType.h"

class Scene {
    public:
    Scene(SceneType sceneType, const char* sceneName, const char* mapPath, int windowWidth, int windowHeight);

    void update(float dt, const SDL_Event &e) {
        world.update(dt, e, type);
    }

    void render() {
        world.render();
    }

    World world;

    const std::string& getName() const { return name; }

private:
    std::string name;
    SceneType type;
    void createProjectile(Vector2D pos, Vector2D dir, int speed);
    void createMapColliders();
    void createItems();
    void createSpawners();
    void createPlayer();
    void createEnemy();
};

#endif //ASSIGNMENT1_SCENE_H