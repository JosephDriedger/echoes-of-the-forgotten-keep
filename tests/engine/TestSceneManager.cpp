//
// Created by Joseph Driedger on 3/17/2026.
//

#include "engine/core/Application.h"
#include "engine/scene/Scene.h"
#include "engine/scene/SceneManager.h"

#include <cassert>
#include <iostream>
#include <memory>
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

    class TestScene final : public engine::Scene
    {
    public:
        TestScene(std::string name, SceneCallData& data, const bool shouldCreate = true)
            : engine::Scene(std::move(name)),
              m_Data(data),
              m_ShouldCreate(shouldCreate)
        {
        }

        bool OnCreate(engine::Application& application) override
        {
            (void)application;
            ++m_Data.CreateCalls;
            return m_ShouldCreate;
        }

        void OnDestroy(engine::Application& application) override
        {
            (void)application;
            ++m_Data.DestroyCalls;
        }

        void OnUpdate(engine::Application& application, const engine::Timestep timestep) override
        {
            (void)application;
            ++m_Data.UpdateCalls;
            m_Data.LastDeltaTime = timestep.GetSeconds();
        }

        void OnRender(engine::Application& application) override
        {
            (void)application;
            ++m_Data.RenderCalls;
        }

    private:
        SceneCallData& m_Data;
        bool m_ShouldCreate;
    };
}

int RunSceneManagerTests()
{
    engine::ApplicationSpecification specification;
    specification.Title = "Test Application";
    specification.Width = 640;
    specification.Height = 360;

    engine::Application application(specification);
    engine::SceneManager sceneManager;

    assert(!sceneManager.HasActiveScene());
    assert(sceneManager.GetCurrentScene() == nullptr);
    assert(sceneManager.GetCurrentSceneName().empty());

    const bool initializeResult = sceneManager.Initialize(application);
    assert(initializeResult);

    SceneCallData firstSceneData;
    sceneManager.ChangeScene(std::make_unique<TestScene>("FirstScene", firstSceneData));

    assert(!sceneManager.HasActiveScene());

    sceneManager.Update(engine::Timestep(0.25));

    assert(sceneManager.HasActiveScene());
    assert(sceneManager.GetCurrentScene() != nullptr);
    assert(sceneManager.GetCurrentSceneName() == "FirstScene");
    assert(firstSceneData.CreateCalls == 1);
    assert(firstSceneData.UpdateCalls == 1);
    assert(firstSceneData.RenderCalls == 0);
    assert(firstSceneData.DestroyCalls == 0);
    assert(firstSceneData.LastDeltaTime == 0.25);

    sceneManager.Render();

    assert(firstSceneData.RenderCalls == 1);

    SceneCallData secondSceneData;
    sceneManager.ChangeScene(std::make_unique<TestScene>("SecondScene", secondSceneData));

    sceneManager.Update(engine::Timestep(0.5));

    assert(firstSceneData.DestroyCalls == 1);
    assert(secondSceneData.CreateCalls == 1);
    assert(secondSceneData.UpdateCalls == 1);
    assert(sceneManager.GetCurrentSceneName() == "SecondScene");

    sceneManager.Render();

    assert(secondSceneData.RenderCalls == 1);

    SceneCallData failedSceneData;
    sceneManager.ChangeScene(std::make_unique<TestScene>("FailedScene", failedSceneData, false));

    sceneManager.Update(engine::Timestep(1.0));

    assert(secondSceneData.DestroyCalls == 1);
    assert(failedSceneData.CreateCalls == 1);
    assert(failedSceneData.UpdateCalls == 0);
    assert(failedSceneData.RenderCalls == 0);
    assert(!sceneManager.HasActiveScene());
    assert(sceneManager.GetCurrentScene() == nullptr);
    assert(sceneManager.GetCurrentSceneName().empty());

    sceneManager.Shutdown();

    assert(!sceneManager.HasActiveScene());
    assert(sceneManager.GetCurrentScene() == nullptr);
    assert(sceneManager.GetCurrentSceneName().empty());

    std::cout << "Scene manager tests passed.\n";
    return 0;
}