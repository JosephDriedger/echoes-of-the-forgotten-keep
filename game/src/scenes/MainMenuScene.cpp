// Created by Adam Van Woerden
/// @file MainMenuScene.cpp
/// @brief Implementation of the title screen with Play, Settings, Credits, Quit.

#include "game/scenes/MainMenuScene.h"

#include "engine/core/Application.h"

#include <SDL3/SDL_keycode.h>
#include <iostream>

namespace game
{
    MainMenuScene::MainMenuScene()
        : engine::Scene("MainMenuScene"),
          m_TextRenderer(),
          m_BackButton(),
          m_QuitYesButton(),
          m_QuitNoButton(),
          m_ScreenWidth(1280),
          m_ScreenHeight(720),
          m_ShowCredits(false),
          m_ShowQuitConfirm(false)
    {
    }

    /// Initializes the text renderer and lays out all menu buttons.
    bool MainMenuScene::OnCreate(engine::Application& application)
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
            std::cerr << "[MainMenuScene] Failed to initialize text renderer\n";
            return false;
        }

        LayoutButtons();

        std::cout << "[MainMenuScene] Created successfully\n";
        return true;
    }

    void MainMenuScene::OnDestroy()
    {
        m_TextRenderer.Destroy();
    }

    /// Creates and positions the main menu buttons, credit-screen Back button,
    /// and quit-confirmation Yes/No buttons.
    void MainMenuScene::LayoutButtons()
    {
        m_Buttons.clear();

        float screenW = static_cast<float>(m_ScreenWidth);
        float startY = static_cast<float>(m_ScreenHeight) * 0.45f;
        float spacing = 80.0f;

        std::vector<std::string> labels = { "Play", "Settings", "Credits", "Quit" };

        for (int i = 0; i < static_cast<int>(labels.size()); i++)
        {
            UIButton btn(labels[i], 0, startY + i * spacing);
            btn.Layout(m_TextRenderer);
            btn.CenterHorizontally(screenW);
            m_Buttons.push_back(btn);
        }

        // Back button for credits screen - bottom left corner
        float margin = 30.0f;
        m_BackButton = UIButton("Back", margin, static_cast<float>(m_ScreenHeight) - 80.0f);
        m_BackButton.Layout(m_TextRenderer);

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

    /// Handles input for the active overlay (quit confirm or credits) or the
    /// main button list. Button clicks request scene changes via the application.
    void MainMenuScene::OnUpdate(engine::Application& application, engine::Timestep timestep)
    {
        (void)timestep;

        const engine::Input& input = application.GetInput();
        int mouseX = input.GetMouseX();
        int mouseY = input.GetMouseY();

        // Quit confirmation overlay
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

        // Credits overlay
        if (m_ShowCredits)
        {
            m_BackButton.UpdateHover(mouseX, mouseY);

            if (m_BackButton.IsClicked(input))
            {
                m_ShowCredits = false;
            }
            return;
        }

        // Main menu
        for (auto& btn : m_Buttons)
        {
            btn.UpdateHover(mouseX, mouseY);
        }

        for (const auto& btn : m_Buttons)
        {
            if (btn.IsClicked(input))
            {
                const std::string& label = btn.GetLabel();

                if (label == "Play")
                {
                    application.RequestSceneChange("GameplayScene");
                    return;
                }
                else if (label == "Settings")
                {
                    application.RequestSceneChange("SettingsScene");
                    return;
                }
                else if (label == "Credits")
                {
                    m_ShowCredits = true;
                }
                else if (label == "Quit")
                {
                    m_ShowQuitConfirm = true;
                }
            }
        }

        if (input.IsKeyPressed(SDLK_ESCAPE))
        {
            m_ShowQuitConfirm = true;
        }
    }

    /// Renders either the main menu, quit confirmation, or credits overlay
    /// depending on current state. Only one overlay is visible at a time.
    void MainMenuScene::OnRender(engine::Application& application)
    {
        (void)application;

        // Title
        float titleScale = 1.5f;
        std::string title = "Echoes of the Forgotten Keep";
        float titleWidth = m_TextRenderer.MeasureTextWidth(title, titleScale);
        float titleX = (static_cast<float>(m_ScreenWidth) - titleWidth) / 2.0f;
        float titleY = static_cast<float>(m_ScreenHeight) * 0.12f;

        m_TextRenderer.RenderText(title, titleX, titleY, titleScale,
                                  0.85f, 0.65f, 0.3f);

        // Quit confirmation
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

        // Credits
        if (m_ShowCredits)
        {
            float creditScale = 0.7f;
            float centerX = static_cast<float>(m_ScreenWidth) / 2.0f;
            float creditY = static_cast<float>(m_ScreenHeight) * 0.35f;
            float lineSpacing = 50.0f;

            std::vector<std::string> credits = {
                "Credits",
                "",
                "Joey Driedger - Architecture Lead",
                "Adam Van Woerden - Rendering Lead",
                "Elijah Fabon - Game Design Lead",
            };

            for (const auto& line : credits)
            {
                if (!line.empty())
                {
                    float w = m_TextRenderer.MeasureTextWidth(line, creditScale);
                    m_TextRenderer.RenderText(line, centerX - w / 2.0f, creditY,
                                              creditScale, 0.8f, 0.8f, 0.8f);
                }
                creditY += lineSpacing;
            }

            m_BackButton.Render(m_TextRenderer);
            return;
        }

        // Menu buttons
        for (const auto& btn : m_Buttons)
        {
            btn.Render(m_TextRenderer);
        }
    }
}
