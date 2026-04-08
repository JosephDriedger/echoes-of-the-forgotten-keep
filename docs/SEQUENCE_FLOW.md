# Sequence Flow

This document describes the runtime flow of the game from launch to shutdown, including the main loop, scene transitions, and per-frame gameplay execution.

## Startup Sequence

```
main()
  └── GameApp::Run()
        └── Application::Run(GameApp)
              │
              ├── 1. Application::Initialize()
              │     ├── SDL_Init(SDL_INIT_VIDEO)
              │     ├── Window::Create("Echoes of the Forgotten Keep", 1280, 720)
              │     └── Renderer::Initialize(window)
              │
              ├── 2. GameApp::OnInitialize(application)
              │     ├── SceneManager::Initialize(application)
              │     └── SceneManager::ChangeScene<MainMenuScene>()
              │           └── MainMenuScene::OnCreate()
              │                 ├── TextRenderer::Initialize(font, shaders, 1280, 720)
              │                 └── LayoutButtons() — Play, Settings, Credits, Quit
              │
              └── 3. Enter main loop
```

## Main Loop (Per Frame)

Every frame follows this exact sequence inside `Application::Run()`:

```
┌─────────────────────────────────────────────────────┐
│ 1. Calculate delta time                             │
│    └── Clamp to 100ms max                           │
│                                                     │
│ 2. Input::BeginFrame()                              │
│    └── Snapshot current keys → previous keys        │
│                                                     │
│ 3. Application::ProcessEvents()                     │
│    └── SDL_PollEvent loop                           │
│        ├── Input::ProcessEvent(event)               │
│        ├── GameApp::OnEvent(event)                  │
│        └── SDL_EVENT_QUIT → RequestQuit()           │
│                                                     │
│ 4. GameApp::OnUpdate(timestep)                      │
│    ├── SceneManager::Update(timestep)               │
│    │   └── Top scene only → Scene::OnUpdate()       │
│    └── Check for deferred scene change request      │
│        └── Route to ChangeScene/PushScene/PopScene  │
│                                                     │
│ 5. Renderer::BeginFrame()                           │
│    └── glClear(COLOR | DEPTH)                       │
│                                                     │
│ 6. GameApp::OnRender()                              │
│    └── SceneManager::Render()                       │
│        └── All scenes bottom-to-top →               │
│            Scene::OnRender()                        │
│                                                     │
│ 7. Renderer::EndFrame()                             │
│    └── SDL_GL_SwapWindow()                          │
└─────────────────────────────────────────────────────┘
```

### Step 1: Delta Time and the 100ms Clamp

Delta time (`dt`) is how many seconds elapsed since the last frame. All movement and animation is multiplied by `dt` to be frame-rate independent (e.g., `position += speed * dt`). The problem is that when the game is paused or the OS hitches, the real elapsed time can be very large (e.g., 5 seconds). If that value is passed to the physics/movement systems, entities would teleport enormous distances in a single frame — a "physics explosion." The 100ms clamp ensures `dt` never exceeds 0.1 seconds, so even after a long pause the game advances by at most one tenth of a second per frame.

### Step 2: Why BeginFrame() Matters for Input

The `Input` class tracks two sets of key states: the **current** frame and the **previous** frame. `IsKeyPressed()` returns true only when a key is down this frame but was not down last frame (a rising edge). `BeginFrame()` copies the current state into the previous state before new SDL events are processed. Without this call, `IsKeyPressed()` would compare the current state against stale data and miss or double-detect key presses.
```

## Scene Transition Flow

### Why Transitions Are Deferred

Scenes cannot switch mid-frame. If a scene destroyed itself during its own `OnUpdate()`, the engine would be executing code inside a deleted object — causing undefined behavior. To prevent this, scenes do not have direct access to the `SceneManager`. Instead, they request a transition by calling:

```cpp
application.RequestSceneChange("PauseMenuScene");
```

This stores the request as a plain string on the `Application` object. Nothing happens immediately. After `SceneManager::Update()` finishes and the current scene's `OnUpdate()` has fully returned, `GameApp::OnUpdate()` checks for a pending request and routes it to the appropriate `SceneManager` method:

```
Scene calls RequestSceneChange("PauseMenuScene")
  │
  ▼
GameApp::OnUpdate() — after SceneManager::Update() returns
  │
  ├── "PopScene"           → SceneManager::PopScene()
  ├── "MainMenuScene"      → SceneManager::ChangeScene<MainMenuScene>()
  ├── "GameplayScene"      → SceneManager::ChangeScene<GameplayScene>()
  ├── "SettingsScene"      → SceneManager::PushScene<SettingsScene>()
  ├── "PauseMenuScene"     → SceneManager::PushScene<PauseMenuScene>()
  └── "PauseSettingsScene" → SceneManager::PushScene<SettingsScene>()
```

### What "PopScene" Means

`"PopScene"` is not the name of an actual scene class. It is a **command string** used within the deferred scene change system. When an overlay scene (like PauseMenuScene or SettingsScene) wants to close itself and return to whatever scene is underneath, it requests `"PopScene"`. `GameApp` recognizes this string and calls `SceneManager::PopScene()` instead of creating a new scene.

This is necessary because overlay scenes do not know what scene is below them. SettingsScene can be opened from either MainMenuScene or PauseMenuScene, so it cannot hard-code a return destination. `"PopScene"` simply removes the top of the stack, revealing whatever is underneath.

### ChangeScene (full replacement)

Destroys every scene in the stack top-to-bottom, then creates the new scene. Used for major transitions like Main Menu to Gameplay, where the old scene's state is no longer needed.

```
ChangeScene<GameplayScene>()
  ├── Destroy all scenes: OnDestroy() each, clear stack
  ├── Create new GameplayScene
  ├── GameplayScene::OnCreate()
  └── Push onto stack (stack now has exactly one scene)
```

### PushScene (overlay)

Pushes a new scene on top without destroying the scene underneath. The lower scene stays alive in memory (preserving all its state) but stops receiving `OnUpdate()` calls, effectively freezing it. Used for pause menus and settings overlays where we need to return to the exact previous state.

```
PushScene<PauseMenuScene>()
  ├── Create new PauseMenuScene
  ├── PauseMenuScene::OnCreate()
  └── Push onto stack (GameplayScene still alive below, frozen)
```

### PopScene (resume)

Destroys only the top scene and removes it from the stack. The scene underneath resumes receiving `OnUpdate()` calls as if nothing happened. All of its state (entity positions, health values, animation progress, etc.) is exactly as it was when the overlay was pushed.

```
PopScene()
  ├── Top scene OnDestroy() — cleans up overlay resources
  └── Pop from stack (scene below resumes updates)
```

## Scene Navigation Map

```
                    ┌──────────────────┐
                    │   MainMenuScene  │
                    │                  │
                    │  Play ──────────────────────────┐
                    │  Settings ──────────┐           │
                    │  Credits (overlay)  │           │
                    │  Quit → confirm     │           │
                    └─────────────────────┘           │
                              │                       │
                         PushScene               ChangeScene
                              │                       │
                              ▼                       ▼
                    ┌──────────────────┐     ┌──────────────────┐
                    │  SettingsScene   │     │  GameplayScene   │
                    │  (overlay)       │     │                  │
                    │                  │     │  ESC ───────────────────┐
                    │  Music slider    │     │                  │     │
                    │  VFX slider      │     └──────────────────┘     │
                    │  Back → PopScene │                          PushScene
                    └──────────────────┘                              │
                                                                      ▼
                                                            ┌──────────────────┐
                                                            │ PauseMenuScene   │
                                                            │ (overlay)        │
                                                            │                  │
                                              PopScene ◄─── │ Resume           │
                                                            │ Settings ──────────────┐
                                      ChangeScene ◄──────── │ Main Menu        │     │
                                      (MainMenuScene)       │ Quit → confirm   │     │
                                                            │ ESC → PopScene   │ PushScene
                                                            └──────────────────┘     │
                                                                                      ▼
                                                                            ┌──────────────────┐
                                                                            │  SettingsScene   │
                                                                            │  (overlay)       │
                                                                            │                  │
                                                                            │  Back → PopScene │
                                                                            │  ESC  → PopScene │
                                                                            └──────────────────┘
```

### Scene Stack Examples

| User Action | Stack (bottom → top) |
|-------------|----------------------|
| Launch game | `[MainMenuScene]` |
| Main Menu → Settings | `[MainMenuScene, SettingsScene]` |
| Settings → Back | `[MainMenuScene]` |
| Main Menu → Play | `[GameplayScene]` |
| Gameplay → ESC | `[GameplayScene, PauseMenuScene]` |
| Pause → Settings | `[GameplayScene, PauseMenuScene, SettingsScene]` |
| Settings → Back | `[GameplayScene, PauseMenuScene]` |
| Pause → Resume | `[GameplayScene]` |
| Pause → Main Menu | `[MainMenuScene]` |

### Rendering with Overlays

`SceneManager::Render()` iterates the stack bottom-to-top, calling `OnRender()` on **every** scene in the stack — not just the top one. This is intentional: it allows the 3D game world to remain visible behind overlay menus.

Without any special handling, this would cause the overlay's UI text and buttons to render directly on top of the fully-lit 3D scene, making them hard to read. To solve this, overlay scenes (PauseMenuScene, SettingsScene) draw a full-screen semi-transparent dark rectangle using the `QuadRenderer` as the first thing in their `OnRender()`, before any UI elements. This dims the scene underneath so the menu text is clearly visible.

```
Stack: [GameplayScene, PauseMenuScene, SettingsScene]

Render order:
  1. GameplayScene::OnRender()    — 3D world, HUD
  2. PauseMenuScene::OnRender()   — dark overlay (75% black), then title + buttons
  3. SettingsScene::OnRender()    — dark overlay (85% black), then title + sliders
```

Note that while **all** scenes in the stack are rendered, only the **top** scene receives `OnUpdate()` calls. This means the gameplay world is frozen (no entity movement, no AI, no combat) while any overlay is active, but it still appears on screen behind the darkened overlay.

## Gameplay Frame (GameplayScene::OnUpdate)

When GameplayScene is the active (top) scene, each frame runs the ECS systems in this order:

```
GameplayScene::OnUpdate(dt)
  │
  ├── Debug systems
  │   ├── DebugToggle::Update(input)      — F3/F4/F5 key checks
  │   └── FPSCounter::Update(timestep)    — track frame rate
  │
  ├── ESC check → RequestSceneChange("PauseMenuScene"), return
  │
  └── ECS system pipeline
      │
      ├──  1. CombatInputSystem     — mouse clicks → attack requests
      ├──  2. MovementSystem        — WASD → position changes
      ├──  3. CollisionSystem       — AABB overlap detection & resolution
      ├──  4. AnimationSystem       — advance skeleton animation, write bone matrices
      ├──  5. BoneAttachmentSystem  — position sword/shield on bones
      ├──  6. AttackHitboxSystem    — track active hitboxes, prevent multi-hit
      ├──  7. DamageSystem          — apply IncomingHit to Health
      ├──  8. DeathSystem           — mark dead, convert collider to trigger, play death SFX
      ├──  9. EnemyAISystem         — Idle/Patrol/Chase/Attack state machine
      ├── 10. HitTimerSystem        — count down i-frame timers
      ├── 11. SwitchTriggerSystem   — detect player on switches
      ├── 12. DoorSystem            — proximity-based door opening, play door SFX
      ├── 13. DoorPuzzleSystem      — open/close doors linked to switches via TriggerId
      ├── 14. LifetimeSystem        — decrement timers, destroy expired entities
      ├── 15. CameraFollowSystem    — smooth camera follow + zoom
      └──     AudioEventQueue::process() — play all queued sound effects
```

### Why This Order Matters

- **Input before movement** (1-2): Player actions are read before applying them
- **Collision after movement** (3): Positions are corrected after being changed
- **Animation after collision** (4): Bones reflect the final resolved position
- **Bone attachments after animation** (5): Sword/shield follow updated bone transforms
- **Hitbox after bones** (6): Attack collision shapes match the animated pose
- **Damage after hitbox** (7): Hits detected this frame are applied immediately
- **Death after damage** (8): Entities killed this frame are marked before AI runs
- **AI after death** (9): Dead entities are skipped by the AI state machine
- **Hit timers after AI** (10): Invincibility frame countdowns tick after all damage and AI processing
- **World systems** (11-14): Switches, proximity doors, puzzle doors, and cleanup run after all combat
- **Camera last** (15): Camera captures the final state of everything
- **Audio last**: All sound effects queued during the frame are played together at the end, ensuring consistent audio timing

## Gameplay Render (GameplayScene::OnRender)

```
GameplayScene::OnRender()
  │
  ├── Bind 3D shader
  │   ├── Set view/projection matrices (camera position and perspective)
  │   └── Upload identity bone matrices (default: no skeletal deformation)
  │
  ├── For each entity with Transform + Render:
  │   ├── Build model matrix (from position/rotation/scale or pre-computed)
  │   ├── Upload bone matrices (if AnimationState present)
  │   │   └── Reset to identity when switching from animated to non-animated
  │   ├── Bind texture
  │   └── Draw mesh
  │
  ├── Debug: collider wireframes (if F3+F5 active)
  │
  ├── UISystem: health bar HUD (switches to orthographic projection)
  │
  └── Debug: FPS overlay text (if F3+F4 active)
```

### Bone Matrices Explained

The vertex shader has an array of 100 `mat4` uniforms called `FinalBoneMatrices`. Each vertex in an animated mesh stores up to 4 bone IDs and weights that indicate which bones influence it and by how much. The shader multiplies the vertex position by the weighted combination of its bone matrices to deform the mesh into the current animation pose (this is called **GPU skinning**).

For non-animated entities (walls, floors, doors), the bone matrices are set to **identity** (no transformation), so the shader passes vertex positions through unchanged. The identity reset is important because uniform values persist between draw calls — without resetting, a wall drawn after an animated character would inherit the character's bone deformations and appear warped.

## Shutdown Sequence

```
Application::Run() — m_IsRunning becomes false
  │
  ├── GameApp::OnShutdown()
  │   └── SceneManager::Shutdown()
  │       └── Destroy all scenes top-to-bottom (OnDestroy each)
  │
  └── Application::Shutdown()
      ├── Renderer::Shutdown()
      ├── Window::Destroy()
      └── SDL_Quit()
```
