//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H

#include "engine/scene/Scene.h"

namespace game
{
    class GameplayScene final : public engine::Scene
    {
    public:
        GameplayScene();

        bool OnCreate(engine::Application& application) override;
        void OnUpdate(engine::Application& application, engine::Timestep timestep) override;
        void OnRender(engine::Application& application) override;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAMEPLAYSCENE_H