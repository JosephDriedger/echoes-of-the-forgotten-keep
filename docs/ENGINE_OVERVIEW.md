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
              └── SceneManager (scene stack)
                    ├── GameplayScene (bottom)
                    ├── PauseMenuScene (overlay)
                    └── SettingsScene  (overlay)
```

### Application Lifecycle

The engine follows a **listener pattern** (also known as the observer or callback pattern). `Application` owns the main loop and core engine resources (window, renderer, input), but it has no knowledge of game-specific code. Instead, `GameApp` implements the `IApplicationListener` interface, and `Application` calls its methods at the appropriate times. This keeps the engine reusable — a different game could provide a different listener without modifying `Application`. The lifecycle callbacks are:

1. **OnInitialize** - Set up the SceneManager, load the first scene
2. **OnEvent** - Process SDL events (input, window resize, quit)
3. **OnUpdate** - Run game logic, then check for deferred scene changes
4. **OnRender** - Draw the active scene
5. **OnShutdown** - Clean up all resources

The main loop in `Application::Run()` handles:
- `Input::BeginFrame()` to snapshot previous-frame key/mouse state for edge detection
- SDL event polling via `ProcessEvents()`
- Timestep calculation with a 100ms clamp to prevent physics explosions after pauses
- Updates via `OnUpdate(timestep)`
- Rendering via `OnRender()`

### Scene Stack and Transitions

The `SceneManager` maintains a **stack of scenes** rather than a single active scene. This allows overlay scenes (pause menu, settings) to be pushed on top without destroying the gameplay scene underneath.

- **`ChangeScene<T>()`** - Destroys all scenes in the stack and creates a new one (used for full transitions like Main Menu to Gameplay)
- **`PushScene<T>()`** - Pushes an overlay scene on top. The scene below stays alive but stops receiving updates.
- **`PopScene()`** - Destroys the top overlay and resumes the scene underneath.
- **Rendering** renders all scenes bottom-to-top, so the gameplay scene is visible behind the pause menu. Overlay scenes (PauseMenuScene, SettingsScene) draw a dark semi-transparent background quad before their UI elements to dim the scene underneath.
- **Updating** only runs the top scene, so gameplay is frozen while paused.

Scenes cannot directly access the `SceneManager`. Instead, they call `Application::RequestSceneChange("SceneName")`, which stores the request. After the current frame's `OnUpdate()` completes, `GameApp` reads the request and routes it:

| Request String         | Action          | Notes                              |
|------------------------|-----------------|------------------------------------|
| `"MainMenuScene"`      | ChangeScene     | Replaces entire stack              |
| `"GameplayScene"`      | ChangeScene     | Replaces entire stack              |
| `"SettingsScene"`      | PushScene       | Pushed over MainMenuScene          |
| `"PauseMenuScene"`     | PushScene       | Pushed over GameplayScene          |
| `"PauseSettingsScene"` | PushScene       | Pushed over PauseMenuScene         |
| `"PopScene"`           | PopScene        | Not a scene — a command to close the top overlay and resume the scene underneath |

`"PopScene"` is not an actual scene class. It is a command string that overlay scenes (PauseMenuScene, SettingsScene) use to close themselves and return to whatever scene is below them on the stack. This is necessary because overlays do not know which scene opened them — SettingsScene can be reached from either MainMenuScene or PauseMenuScene.

This deferred approach prevents destroying the active scene while it is still executing.

## Rendering Pipeline

### Core Rendering

The engine uses OpenGL 4.6 (loaded via GLAD) with a **forward rendering pipeline** — each object is drawn once per frame with its final lighting and texturing applied in a single pass. This is the simplest rendering approach (as opposed to deferred rendering, which separates geometry and lighting into multiple passes) and is well-suited for a scene with a moderate number of objects:

- **Shader** - Compiles GLSL vertex/fragment programs, caches uniform locations for performance
- **Mesh** - GPU-resident vertex data (position, normal, UV, bone IDs/weights) with VAO/VBO/EBO
- **Texture** - 2D textures loaded from PNG files via SDL3_image
- **Camera** - Supports both perspective (gameplay) and orthographic (UI) projections
- **RenderSystem** - Utility for drawing meshes with shaders and optional textures

### Skeletal Animation

Full skeletal animation support via Assimp model loading. Skeletal animation works by defining a hierarchy of "bones" inside a 3D model. Each vertex is bound to one or more bones with weights (e.g., a wrist vertex might be 80% influenced by the hand bone and 20% by the forearm bone). When bones move, the vertices follow — this is how a character mesh deforms when walking, attacking, or dying. The animation data (bone positions at each point in time) is loaded from GLB files alongside the model.

- **Skeleton** - Tree of bones with parent-child relationships (e.g., shoulder → upper arm → forearm → hand) and offset matrices that transform from model space to bone space
- **AnimationClip** - A named animation (e.g., "idle", "run", "attack1") containing keyframes for each bone's position, rotation, and scale at specific timestamps
- **Animator** - Evaluates a clip at a given time by interpolating between the two nearest keyframes for each bone, producing smooth motion between poses
- **AnimationState** (component) - Tracks which clip is playing, the current playback time, and outputs `FinalBoneMatrices[100]` — an array of 4x4 matrices uploaded to the vertex shader each frame for GPU skinning (the GPU applies bone transformations to vertices, which is much faster than doing it on the CPU)

### UI Rendering

Two specialized renderers handle 2D overlays:

- **TextRenderer** - Rasterizes TrueType fonts into a glyph atlas (512x512, ASCII 32-126) using stb_truetype. Renders text as per-glyph textured quads with alpha blending.
- **QuadRenderer** - Draws solid-color rectangles for UI backgrounds, slider tracks, and health bars.

Both use **orthographic projection** with a top-left origin, meaning screen coordinates map directly to pixels (e.g., `(100, 50)` means 100 pixels from the left, 50 pixels from the top). This differs from the **perspective projection** used by the 3D gameplay camera, where distant objects appear smaller. Orthographic projection is standard for 2D UI because text and buttons should always be the same size regardless of camera position.

Because the 3D renderer leaves OpenGL in a specific state (depth testing enabled, face culling on, blending off), both UI renderers save the current GL state before drawing, disable depth testing and face culling, enable alpha blending, and then restore the previous state when done. This prevents UI rendering from interfering with subsequent 3D draws.

## Input System

The `Input` class wraps SDL3 event handling with frame-based state tracking:

- **Keyboard**: `IsKeyDown()` (held), `IsKeyPressed()` (just pressed this frame), `IsKeyReleased()`
- **Mouse**: Button state, position (`GetMouseX/Y()`), delta movement, scroll wheel
- **Quit detection**: `IsQuitRequested()` for window close events

`BeginFrame()` is called at the start of each frame to copy current state to previous state, enabling accurate press/release edge detection. Without this, `IsKeyPressed()` would not reliably detect single-frame key presses.

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
│   │   ├── components/         ECS components (all in Components.h)
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
