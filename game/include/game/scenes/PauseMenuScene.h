/// @file PauseMenuScene.h
/// @brief In-game pause menu with Resume, Settings, Main Menu, and Quit.
///
/// Opened by pressing ESC during gameplay. The pause menu replaces
/// the gameplay scene (game state is recreated on resume). Pressing
/// ESC again or clicking Resume returns to GameplayScene.
///
/// Navigation:
///   Resume    -> GameplayScene
///   Settings  -> SettingsScene (returns here via PauseSettingsScene route)
///   Main Menu -> MainMenuScene
///   Quit      -> "Are you sure?" confirmation with Yes/No
///   ESC       -> resume (or dismiss quit confirmation)

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_PAUSEMENUSCENE_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_PAUSEMENUSCENE_H

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
    class PauseMenuScene final : public engine::Scene
    {
    public:
        PauseMenuScene();

        bool OnCreate(engine::Application& application) override;
        void OnDestroy() override;
        void OnUpdate(engine::Application& application, engine::Timestep timestep) override;
        void OnRender(engine::Application& application) override;

    private:
        void LayoutButtons();

        engine::TextRenderer m_TextRenderer;

        std::vector<UIButton> m_Buttons;

        UIButton m_QuitYesButton;
        UIButton m_QuitNoButton;

        int m_ScreenWidth;
        int m_ScreenHeight;

        bool m_ShowQuitConfirm;
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_PAUSEMENUSCENE_H
