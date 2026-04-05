// Created by Adam Van Woerden
/// @file SettingsScene.h
/// @brief Settings screen with Music and VFX volume sliders.
///
/// Reachable from both MainMenuScene and PauseMenuScene. The Back
/// button returns to whichever scene opened it, tracked by
/// GameSettings::SettingsReturnScene (set by GameApp before the
/// scene transition).

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_SETTINGSSCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_SETTINGSSCENE_H

#include "engine/scene/Scene.h"
#include "engine/rendering/TextRenderer.h"
#include "engine/rendering/QuadRenderer.h"
#include "engine/resources/AudioManager.h"

#include "game/ui/UIButton.h"
#include "game/ui/UISlider.h"

namespace engine
{
    class Application;
}

namespace game
{
    class SettingsScene final : public engine::Scene
    {
    public:
        SettingsScene();

        bool OnCreate(engine::Application& application) override;
        void OnDestroy() override;
        void OnUpdate(engine::Application& application, engine::Timestep timestep) override;
        void OnRender(engine::Application& application) override;

    private:
        void LayoutUI();

        engine::TextRenderer m_TextRenderer;
        engine::QuadRenderer m_QuadRenderer;

        UISlider m_MusicSlider;
        UISlider m_VFXSlider;
        UIButton m_BackButton;

        int m_ScreenWidth;
        int m_ScreenHeight;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_SETTINGSSCENE_H
