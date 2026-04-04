// Created by Adam Van Woerden

#include "game/scenes/PauseMenuScene.h"

#include "engine/core/Application.h"

#include <SDL3/SDL_keycode.h>
#include <iostream>

namespace game
{
    PauseMenuScene::PauseMenuScene()
        : engine::Scene("PauseMenuScene"),
          m_TextRenderer(),
          m_QuitYesButton(),
          m_QuitNoButton(),
          m_ScreenWidth(1280),
          m_ScreenHeight(720),
          m_ShowQuitConfirm(false)
    {
    }

    bool PauseMenuScene::OnCreate(engine::Application& application)
    {
        const auto& spec = application.GetSpecification();
        m_ScreenWidth = spec.Width;
        m_ScreenHeight = spec.Height;

        if (!m_TextRenderer.Initialize(
                "asset/fonts/BreatheFireIi-2z9W.ttf",
                64.0f,
                "asset/shaders/text_vertex.glsl",
                "asset/shaders/text_fragment.glsl",
                m_ScreenWidth, m_ScreenHeight))
        {
            std::cerr << "[PauseMenuScene] Failed to initialize text renderer\n";
            return false;
        }

        if (!m_QuadRenderer.Initialize(
                "asset/shaders/quad_vertex.glsl",
                "asset/shaders/quad_fragment.glsl",
                m_ScreenWidth, m_ScreenHeight))
        {
            std::cerr << "[PauseMenuScene] Failed to initialize quad renderer\n";
            return false;
        }

        LayoutButtons();

        std::cout << "[PauseMenuScene] Created successfully\n";
        return true;
    }

    void PauseMenuScene::OnDestroy()
    {
        m_TextRenderer.Destroy();
        m_QuadRenderer.Destroy();
    }

    void PauseMenuScene::LayoutButtons()
    {
        m_Buttons.clear();

        float screenW = static_cast<float>(m_ScreenWidth);
        float startY = static_cast<float>(m_ScreenHeight) * 0.40f;
        float spacing = 80.0f;

        std::vector<std::string> labels = { "Resume", "Settings", "Main Menu", "Quit" };

        for (int i = 0; i < static_cast<int>(labels.size()); i++)
        {
            UIButton btn(labels[i], 0, startY + i * spacing);
            btn.Layout(m_TextRenderer);
            btn.CenterHorizontally(screenW);
            m_Buttons.push_back(btn);
        }

        // Quit confirmation buttons
        float confirmY = static_cast<float>(m_ScreenHeight) * 0.55f;
        float gap = 40.0f;

        m_QuitYesButton = UIButton("Yes", 0, confirmY);
        m_QuitYesButton.Layout(m_TextRenderer);
        m_QuitNoButton = UIButton("No", 0, confirmY);
        m_QuitNoButton.Layout(m_TextRenderer);

        float totalWidth = m_QuitYesButton.GetWidth() + gap + m_QuitNoButton.GetWidth();
        float startX = (screenW - totalWidth) / 2.0f;
        m_QuitYesButton.SetPosition(startX, confirmY);
        m_QuitNoButton.SetPosition(startX + m_QuitYesButton.GetWidth() + gap, confirmY);
    }

    void PauseMenuScene::OnUpdate(engine::Application& application, engine::Timestep timestep)
    {
        (void)timestep;

        const engine::Input& input = application.GetInput();
        int mouseX = input.GetMouseX();
        int mouseY = input.GetMouseY();

        // Quit confirmation
        if (m_ShowQuitConfirm)
        {
            m_QuitYesButton.UpdateHover(mouseX, mouseY);
            m_QuitNoButton.UpdateHover(mouseX, mouseY);

            if (m_QuitYesButton.IsClicked(input))
            {
                application.RequestQuit();
            }
            else if (m_QuitNoButton.IsClicked(input))
            {
                m_ShowQuitConfirm = false;
            }

            if (input.IsKeyPressed(SDLK_ESCAPE))
            {
                m_ShowQuitConfirm = false;
            }
            return;
        }

        // Pause menu buttons
        for (auto& btn : m_Buttons)
        {
            btn.UpdateHover(mouseX, mouseY);
        }

        // ESC to resume
        if (input.IsKeyPressed(SDLK_ESCAPE))
        {
            application.RequestSceneChange("PopScene");
            return;
        }

        for (const auto& btn : m_Buttons)
        {
            if (btn.IsClicked(input))
            {
                const std::string& label = btn.GetLabel();

                if (label == "Resume")
                {
                    application.RequestSceneChange("PopScene");
                    return;
                }
                else if (label == "Settings")
                {
                    application.RequestSceneChange("PauseSettingsScene");
                    return;
                }
                else if (label == "Main Menu")
                {
                    application.RequestSceneChange("MainMenuScene");
                    return;
                }
                else if (label == "Quit")
                {
                    m_ShowQuitConfirm = true;
                }
            }
        }
    }

    void PauseMenuScene::OnRender(engine::Application& application)
    {
        (void)application;

        // Dark overlay background
        m_QuadRenderer.DrawFilledRect(0.0f, 0.0f,
            static_cast<float>(m_ScreenWidth), static_cast<float>(m_ScreenHeight),
            0.0f, 0.0f, 0.0f, 0.75f);

        // Title
        float titleScale = 1.2f;
        std::string title = "Paused";
        float titleWidth = m_TextRenderer.MeasureTextWidth(title, titleScale);
        float titleX = (static_cast<float>(m_ScreenWidth) - titleWidth) / 2.0f;
        float titleY = static_cast<float>(m_ScreenHeight) * 0.15f;

        m_TextRenderer.RenderText(title, titleX, titleY, titleScale,
                                  0.85f, 0.65f, 0.3f);

        if (m_ShowQuitConfirm)
        {
            float promptScale = 0.8f;
            std::string prompt = "Are you sure you want to quit?";
            float promptWidth = m_TextRenderer.MeasureTextWidth(prompt, promptScale);
            float promptX = (static_cast<float>(m_ScreenWidth) - promptWidth) / 2.0f;
            float promptY = static_cast<float>(m_ScreenHeight) * 0.40f;

            m_TextRenderer.RenderText(prompt, promptX, promptY, promptScale,
                                      0.9f, 0.9f, 0.9f);

            m_QuitYesButton.Render(m_TextRenderer);
            m_QuitNoButton.Render(m_TextRenderer);
            return;
        }

        // Menu buttons
        for (const auto& btn : m_Buttons)
        {
            btn.Render(m_TextRenderer);
        }
    }
}
