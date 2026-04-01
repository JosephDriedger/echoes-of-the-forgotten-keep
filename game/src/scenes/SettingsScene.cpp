#include "game/scenes/SettingsScene.h"

#include "engine/core/Application.h"
#include "game/GameSettings.h"

#include <SDL3/SDL_keycode.h>
#include <iostream>

namespace game
{
    SettingsScene::SettingsScene()
        : engine::Scene("SettingsScene"),
          m_TextRenderer(),
          m_QuadRenderer(),
          m_MusicSlider(),
          m_VFXSlider(),
          m_BackButton(),
          m_ScreenWidth(1280),
          m_ScreenHeight(720)
    {
    }

    bool SettingsScene::OnCreate(engine::Application& application)
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
            std::cerr << "[SettingsScene] Failed to initialize text renderer\n";
            return false;
        }

        if (!m_QuadRenderer.Initialize(
                "asset/shaders/quad_vertex.glsl",
                "asset/shaders/quad_fragment.glsl",
                m_ScreenWidth, m_ScreenHeight))
        {
            std::cerr << "[SettingsScene] Failed to initialize quad renderer\n";
            return false;
        }

        LayoutUI();

        std::cout << "[SettingsScene] Created successfully\n";
        return true;
    }

    void SettingsScene::OnDestroy()
    {
        m_TextRenderer.Destroy();
        m_QuadRenderer.Destroy();
    }

    void SettingsScene::LayoutUI()
    {
        float centerX = static_cast<float>(m_ScreenWidth) / 2.0f;
        float trackWidth = 300.0f;
        float labelOffset = 200.0f;

        GameSettings& settings = GameSettings::Instance();

        m_MusicSlider = UISlider("Music", 0, 0, trackWidth,
                                 &settings.MusicVolume, 0.0f, 1.0f);
        m_VFXSlider = UISlider("VFX", 0, 0, trackWidth,
                               &settings.VFXVolume, 0.0f, 1.0f);

        // Position sliders centered
        float sliderStartX = centerX - (labelOffset + trackWidth) / 2.0f;
        float sliderY = static_cast<float>(m_ScreenHeight) * 0.38f;
        float sliderSpacing = 70.0f;

        m_MusicSlider.SetPosition(sliderStartX, sliderY);
        m_VFXSlider.SetPosition(sliderStartX, sliderY + sliderSpacing);

        // Back button
        m_BackButton = UIButton("Back", 0, static_cast<float>(m_ScreenHeight) * 0.75f);
        m_BackButton.Layout(m_TextRenderer);
        m_BackButton.CenterHorizontally(static_cast<float>(m_ScreenWidth));
    }

    void SettingsScene::OnUpdate(engine::Application& application, engine::Timestep timestep)
    {
        (void)timestep;

        const engine::Input& input = application.GetInput();
        int mouseX = input.GetMouseX();
        int mouseY = input.GetMouseY();

        m_MusicSlider.Update(input);
        m_VFXSlider.Update(input);

        m_BackButton.UpdateHover(mouseX, mouseY);

        if (m_BackButton.IsClicked(input))
        {
            application.RequestSceneChange("MainMenuScene");
        }
    }

    void SettingsScene::OnRender(engine::Application& application)
    {
        (void)application;

        // Title
        float titleScale = 1.2f;
        std::string title = "Settings";
        float titleWidth = m_TextRenderer.MeasureTextWidth(title, titleScale);
        float titleX = (static_cast<float>(m_ScreenWidth) - titleWidth) / 2.0f;
        float titleY = static_cast<float>(m_ScreenHeight) * 0.12f;

        m_TextRenderer.RenderText(title, titleX, titleY, titleScale,
                                  0.85f, 0.65f, 0.3f);

        // Sliders
        m_MusicSlider.Render(m_TextRenderer, m_QuadRenderer);
        m_VFXSlider.Render(m_TextRenderer, m_QuadRenderer);

        // Back button
        m_BackButton.Render(m_TextRenderer);
    }
}
