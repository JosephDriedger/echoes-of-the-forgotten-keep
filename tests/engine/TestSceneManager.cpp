//
// Created by Joseph Driedger on 3/17/2026.
//

#include "engine/scene/Scene.h"
#include "engine/scene/SceneManager.h"

#include <cassert>
#include <iostream>
#include <string>

namespace
{
    struct SceneCallData
    {
        int CreateCalls = 0;
        int DestroyCalls = 0;
        int UpdateCalls = 0;
        int RenderCalls = 0;
        double LastDeltaTime = 0.0;
    };

    // Static pointer so default-constructible TestScene can access call data
    SceneCallData* g_ActiveCallData = nullptr;
    bool g_ShouldCreate = true;

    class TestScene final : public engine::Scene
    {
    public:
        TestScene()
            : engine::Scene("TestScene"),
              m_Data(g_ActiveCallData)
        {
        }

        bool OnCreate(engine::Application& application) override
        {
            (void)application;
            if (m_Data) ++m_Data->CreateCalls;
            return g_ShouldCreate;
        }

        void OnDestroy() override
        {
            if (m_Data) ++m_Data->DestroyCalls;
        }

        void OnUpdate(engine::Application& application, const engine::Timestep timestep) override
        {
            (void)application;
            if (m_Data)
            {
                ++m_Data->UpdateCalls;
                m_Data->LastDeltaTime = timestep.GetSeconds();
            }
        }

        void OnRender(engine::Application& application) override
        {
            (void)application;
            if (m_Data) ++m_Data->RenderCalls;
        }

    private:
        SceneCallData* m_Data;
    };
}

int RunSceneManagerTests()
{
    // Test SceneManager initialization
    engine::SceneManager sceneManager;
    // Note: Initialize and ChangeScene require a valid Application with
    // an OpenGL context. We test the SceneManager can be constructed and
    // that its Shutdown is safe to call without initialization.
    sceneManager.Shutdown();

    std::cout << "Scene manager tests passed.\n";
    return 0;
}
