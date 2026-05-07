# Echoes of the Forgotten Keep

A 3D dungeon-crawler action game built on a custom C++20 ECS game engine with OpenGL 4.6 rendering, skeletal animation, and procedural dungeon generation.

## Engine Overview

The engine follows a modular architecture with a clean separation between the `engine/` library (reusable core) and the `game/` library (game-specific logic).

### Key Features

- **Entity-Component-System** - Data-oriented ECS with a central Registry, contiguous component storage, and signature-based entity filtering. Supports up to 5000 entities and 64 component types.
- **OpenGL 4.6 Rendering** - Forward rendering pipeline with GLSL shaders, texture mapping, and alpha blending.
- **Skeletal Animation** - Full bone hierarchy support with keyframe interpolation, animation blending, and GPU skinning via bone matrices. Animations are loaded from GLB files via Assimp.
- **TrueType Text Rendering** - Font atlas rasterization using stb_truetype with per-glyph textured quads.
- **SDL3 Platform Layer** - Cross-platform windowing, input handling (keyboard + mouse with frame-edge detection), and event processing.
- **Scene Management** - Scene stack with push/pop for overlay menus (pause, settings) and deferred transitions to prevent mid-update destruction.
- **Resource Management** - Cached loading for shaders, textures, meshes, and animations with path-based deduplication.
- **Procedural Dungeon Generation** - Seed-based room placement with a prefab system for walls, floors, doors, and props.
- **UI System** - Reusable UIButton and UISlider widgets with orthographic rendering over the 3D scene.

### ECS Components

| Component | Purpose |
|-----------|---------|
| Transform | Position, rotation, scale in 3D space |
| Render | Mesh + texture references for drawing |
| Player | Marker identifying the player entity |
| Health | Current and maximum hit points |
| Collider | AABB collision volume with trigger/static flags |
| AnimationState | Skeletal animation playback and bone matrices |
| CombatState | Attack state, combo chain, incoming damage, lunge, knockback |
| AttackStateComponent | Per-attacker hitbox entity and per-swing hit tracking (owned by AttackHitboxSystem) |
| CameraFollowState | Camera zoom level for the followed entity (owned by CameraFollowSystem) |
| EnemyAI | AI behavior state machine (Idle/Patrol/Chase/Attack), knockback |
| Door | Door swing state and proximity-based opening |
| Switch | Floor switch for puzzle interactions |
| BoneAttachment | Attaches entity to a parent's skeleton bone |
| Lifetime | Auto-destroy timer |

### ECS Systems

| System | Purpose |
|--------|---------|
| MovementSystem | WASD player movement with facing direction |
| CollisionSystem | AABB overlap detection and resolution |
| AnimationSystem | Skeleton evaluation and bone matrix computation |
| BoneAttachmentSystem | Positions bone-attached entities (sword, shield) |
| CombatInputSystem | Mouse input to attack/combo requests |
| AttackHitboxSystem | Active hitbox tracking, multi-hit prevention |
| DamageSystem | Applies incoming hits to health |
| EnemyAISystem | AI state machine execution |
| HitTimerSystem | Invincibility frame countdown after taking damage |
| DungeonSpawnSystem | Procedural room and entity generation |
| DoorSystem | Proximity-based door animation |
| SwitchTriggerSystem | Player-switch collision detection |
| CameraFollowSystem | Smooth camera tracking with zoom |
| UISystem | HUD rendering (health bar) |
| LifetimeSystem | Entity expiration and cleanup |
| DebugColliderRenderer | Wireframe collider visualization |
| FPSCounter | Frame rate tracking |

For detailed documentation, see the `docs/` folder:
- [Engine Overview](docs/ENGINE_OVERVIEW.md) - Architecture, rendering pipeline, and build system
- [ECS Architecture](docs/ECS_ARCHITECTURE.md) - Components, systems, and execution order
- [Game Design](docs/GAME_DESIGN.md) - Gameplay mechanics, controls, and design decisions
- [Sequence Flow](docs/SEQUENCE_FLOW.md) - Runtime flow from startup to shutdown, scene transitions, and per-frame execution order

## Requirements

- CMake 4.1 or newer
- C++20 compiler

## Dependencies

| Library | Purpose |
|---------|---------|
| SDL3 | Windowing, events, platform abstraction |
| SDL3_image | Image loading (PNG) |
| OpenGL 4.6 | GPU rendering |
| GLAD | OpenGL function loader |
| GLM | Math (vectors, matrices, quaternions) |
| Assimp 6.0.4 | 3D model and animation loading (GLB/GLTF) |
| stb_truetype | TrueType font rasterization |

SDL3 and GLM are downloaded automatically via CMake `FetchContent` if not found on the system.

## Build

### Debug (CLion)

Open the project in CLion and use the built-in **Debug** run configuration.

### Release (Windows)

Requires CLion 2025.3.1 installed (provides bundled CMake and MinGW).

**1. Configure and build:**

```powershell
$cmake = "C:\Program Files\JetBrains\CLion 2025.3.1\bin\cmake\win\x64\bin\cmake.exe"
$mingw  = "C:\Program Files\JetBrains\CLion 2025.3.1\bin\mingw\bin"

& $cmake -B cmake-build-release -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER="$mingw\gcc.exe" -DCMAKE_CXX_COMPILER="$mingw\g++.exe"
& $cmake --build cmake-build-release --target echoes_of_the_forgotten_keep
```

**2. Copy MinGW runtime DLLs into the output folder:**

```powershell
Copy-Item "$mingw\libgcc_s_seh-1.dll", "$mingw\libstdc++-6.dll", "$mingw\libwinpthread-1.dll" -Destination "cmake-build-release\bin"
```

**3. Build the installer:**

Open `installer.iss` in [Inno Setup](https://jrsoftware.org/isdl.php) and compile it.
The finished installer is written to `installer-output/EchoesOfTheForgottenKeep-Setup.exe`.

## Controls

| Input | Action |
|-------|--------|
| W/A/S/D or Arrow Keys | Move player |
| Space | Attack (3-hit combo chain) |
| Mouse Wheel | Camera zoom |
| ESC | Pause menu |
| F3 | Toggle debug mode (enables F4/F5) |
| F4 | Toggle FPS display (requires F3) |
| F5 | Toggle collider visualization (requires F3) |

## Team

- **Joey Driedger** - Architecture Lead
- **Adam Van Woerden** - Rendering Lead
- **Elijah Fabon** - Game Design Lead
