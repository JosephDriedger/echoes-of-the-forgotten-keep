// Created by Adam Van Woerden
/// @file MainMenuScene.h
/// @brief Title screen with Play, Settings, Credits, and Quit buttons.
///
/// The main menu is the first scene shown when the game launches.
/// It renders the game title and a vertical list of UIButtons.
/// Sub-screens (Credits, Quit confirmation) are handled as overlay
/// states within this scene rather than separate scenes.
///
/// Navigation:
///   Play     -> GameplayScene
///   Settings -> SettingsScene (returns here via GameSettings::SettingsReturnScene)
///   Credits  -> inline overlay with Back button (bottom-left)
///   Quit     -> "Are you sure?" confirmation with Yes/No
///   ESC      -> shows quit confirmation

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_MAINMENUSCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_MAINMENUSCENE_H

#include "engine/scene/Scene.h"
#include "engine/rendering/TextRenderer.h"

#include "game/ui/UIButton.h"

#include <string>
#include <vector>

namespace engine
{
    class Application;
}

namespace game
{
    class MainMenuScene final : public engine::Scene
    {
    public:
        MainMenuScene();

        bool OnCreate(engine::Application& application) override;
        void OnDestroy() override;
        void OnUpdate(engine::Application& application, engine::Timestep timestep) override;
        void OnRender(engine::Application& application) override;

    private:
        void LayoutButtons();

        engine::TextRenderer m_TextRenderer;

        std::vector<UIButton> m_Buttons;
        UIButton m_BackButton;

        UIButton m_QuitYesButton;
        UIButton m_QuitNoButton;

        int m_ScreenWidth;
        int m_ScreenHeight;

        bool m_ShowCredits;
        bool m_ShowQuitConfirm;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_MAINMENUSCENE_H
