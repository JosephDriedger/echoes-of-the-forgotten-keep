# Engine Overview

## Introduction

**Echoes of the Forgotten Keep** is built on a custom C++20 game engine featuring an Entity-Component-System (ECS) architecture, OpenGL 4.6 rendering with skeletal animation support, and SDL3-based windowing and input. The engine is designed to be modular: core systems live in the `engine/` library while game-specific logic lives in the `game/` library.

## Architecture

```
main.cpp
  └── GameApp (IApplicationListener)
        └── Application (engine loop)
              ├── Window (SDL3)
              ├── Renderer (OpenGL 4.6)
              ├── Input (keyboard + mouse)
              └── SceneManager
                    └── Active Scene
                          ├── Registry (ECS world)
                          ├── Systems (game logic)
                          └── Resource Managers
```

### Application Lifecycle

The engine follows a listener pattern. `Application` owns the main loop, and `GameApp` implements `IApplicationListener` to receive lifecycle callbacks:

1. **OnInitialize** - Set up the SceneManager, load the first scene
2. **OnEvent** - Process SDL events (input, window resize, quit)
3. **OnUpdate** - Run game logic, then check for deferred scene changes
4. **OnRender** - Draw the active scene
5. **OnShutdown** - Clean up all resources

The main loop in `Application::Run()` handles:
- SDL event polling via `ProcessEvents()`
- Fixed-timestep updates via `OnUpdate(timestep)`
- Rendering via `OnRender()`
- Frame timing to maintain 60 FPS target

### Deferred Scene Transitions

Scenes cannot directly access the `SceneManager`. Instead, they call `Application::RequestSceneChange("SceneName")`, which stores the request. After the current frame's `OnUpdate()` completes, `GameApp` reads the request and routes it to the correct scene type:

| Request String       | Scene Created   | Notes                              |
|---------------------|-----------------|------------------------------------|
| `"MainMenuScene"`   | MainMenuScene   |                                    |
| `"GameplayScene"`   | GameplayScene   |                                    |
| `"SettingsScene"`   | SettingsScene   | Returns to MainMenuScene           |
| `"PauseMenuScene"`  | PauseMenuScene  |                                    |
| `"PauseSettingsScene"` | SettingsScene | Returns to PauseMenuScene          |

This deferred approach prevents destroying the active scene while it is still executing.

## Rendering Pipeline

### Core Rendering

The engine uses OpenGL 4.6 (loaded via GLAD) with a forward rendering pipeline:

- **Shader** - Compiles GLSL vertex/fragment programs, caches uniform locations for performance
- **Mesh** - GPU-resident vertex data (position, normal, UV, bone IDs/weights) with VAO/VBO/EBO
- **Texture** - 2D textures loaded from PNG files via SDL3_image
- **Camera** - Supports both perspective (gameplay) and orthographic (UI) projections
- **RenderSystem** - Utility for drawing meshes with shaders and optional textures

### Skeletal Animation

Full skeletal animation support via Assimp model loading:

- **Skeleton** - Bone hierarchy with parent-child relationships and offset matrices
- **AnimationClip** - Named animation containing per-bone keyframes (position, rotation, scale)
- **Animator** - Evaluates clips over time, interpolates between keyframes
- **AnimationState** (component) - Tracks current clip, blend time, and outputs `FinalBoneMatrices[100]` to the vertex shader for GPU skinning

### UI Rendering

Two specialized renderers handle 2D overlays:

- **TextRenderer** - Rasterizes TrueType fonts into a glyph atlas (512x512, ASCII 32-126) using stb_truetype. Renders text as per-glyph textured quads with alpha blending.
- **QuadRenderer** - Draws solid-color rectangles for UI backgrounds, slider tracks, and health bars.

Both use orthographic projection (top-left origin) and save/restore GL state (depth test, cull face, blend) to coexist with the 3D renderer.

## Input System

The `Input` class wraps SDL3 event handling with frame-based state tracking:

- **Keyboard**: `IsKeyDown()` (held), `IsKeyPressed()` (just pressed this frame), `IsKeyReleased()`
- **Mouse**: Button state, position (`GetMouseX/Y()`), delta movement, scroll wheel
- **Quit detection**: `IsQuitRequested()` for window close events

Previous-frame state is stored to detect press/release edges.

## Resource Management

Three manager classes handle asset caching:

- **ShaderManager** - Compiles and caches GLSL shader programs by name
- **TextureManager** - Loads and caches PNG textures by file path
- **MeshManager** - Loads and caches 3D models (GLB/GLTF via Assimp), extracts skeleton data
- **AnimationLoader** - Loads animation clips from GLB files, appends to shared clip vectors
- **AssetManager** - Aggregates ShaderManager and TextureManager for convenient access

All managers use path-based deduplication to avoid loading the same asset twice.

## Build System

- **Language**: C++20
- **Build tool**: CMake 4.1+
- **Compiler**: MinGW (GCC)
- **Output**: `libengine.a` + `libgame.a` + `echoes_of_the_forgotten_keep.exe`
- **Assets**: Copied post-build to the output directory
- **DLLs**: SDL3.dll, SDL3_image.dll, libassimp-6.dll copied alongside the executable

### Third-Party Dependencies

| Library       | Purpose                                    |
|---------------|--------------------------------------------|
| SDL3          | Window creation, events, platform layer    |
| SDL3_image    | PNG/image file loading                     |
| OpenGL 4.6    | GPU rendering                              |
| GLAD          | OpenGL function loader                     |
| GLM           | Math library (vectors, matrices, quaternions) |
| Assimp 6.0.4  | 3D model and animation loading (GLB/GLTF) |
| stb_truetype  | TrueType font rasterization                |

SDL3 and GLM are fetched automatically via CMake `FetchContent` if not found on the system.

## Directory Structure

```
echoes-of-the-forgotten-keep/
├── main.cpp                    Entry point
├── engine/                     Engine library
│   ├── include/engine/
│   │   ├── core/               Application, Window, Timestep
│   │   ├── ecs/                Entity, Component, Registry, System, Signature
│   │   ├── input/              Input handling
│   │   ├── platform/           Platform-specific (Window)
│   │   ├── rendering/          Shader, Mesh, Texture, Camera, Renderers, Animation
│   │   ├── resources/          Asset/Shader/Texture/Mesh managers
│   │   └── scene/              Scene, SceneManager, PrefabManager
│   └── src/                    Implementation files (mirrors include structure)
├── game/                       Game library
│   ├── include/game/
│   │   ├── components/         ECS components (Transform, Health, etc.)
│   │   ├── scenes/             Scene implementations
│   │   ├── systems/            ECS systems (Movement, Combat, etc.)
│   │   └── ui/                 UI widgets (UIButton, UISlider)
│   └── src/                    Implementation files
├── asset/                      Game assets
│   ├── animations/             Character animation GLB files
│   ├── characters/             Character models and textures
│   ├── dungeon/                Dungeon props (walls, floors, doors)
│   ├── equipment/              Weapons and shields
│   ├── fonts/                  TrueType font files
│   ├── shaders/                GLSL shader source files
│   ├── sounds/                 Audio files
│   └── ui/                     UI element assets
├── third_party/                External libraries (Assimp, GLAD, stb)
├── tests/                      Unit tests
└── docs/                       Documentation
```
