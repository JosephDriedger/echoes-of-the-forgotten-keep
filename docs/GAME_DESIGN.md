# Game Design Document

## Game Concept

**Echoes of the Forgotten Keep** is a 3D dungeon-crawler action game where the player controls a knight exploring procedurally generated dungeons. The game features real-time melee combat with a combo system, enemy AI, interactive doors, and skeletal animation.

## Gameplay Loop

1. **Explore** - Navigate through procedurally generated dungeon rooms
2. **Fight** - Engage skeleton enemies in melee combat using a 3-hit combo system
3. **Survive** - Manage health while fighting enemies and navigating the dungeon

## Player Character

- **Model**: Knight with sword (right hand) and shield (left hand)
- **Movement**: WASD or Arrow Keys with automatic facing direction
- **Combat**: Space to attack, 3-hit combo chain (horizontal slice, stab, jump chop)
- **Health**: 10 HP, displayed as a health bar HUD element

## Combat System

The combat system uses a combo chain with timed input windows:

```
Click → Attack 1 (Horizontal Slice)
  └─ Click during combo window → Attack 2 (Stab)
       └─ Click during combo window → Attack 3 (Jump Chop)
```

- Each attack has a damage value and its own animation clip
- A **combo window** is a brief time period near the end of each attack animation during which the player can click again to chain into the next attack. If the player clicks during this window, the next attack in the sequence begins immediately. If the player does not click before the window closes, the combo resets to the beginning (Attack 1).
- Enemies have their own AI-driven attack patterns with cooldown timers

## Enemy AI

Enemies use a state machine with four states:

- **Idle** - Stationary, waiting for the player to enter vision range
- **Patrol** - Walks between two waypoints (if configured)
- **Chase** - Moves toward the player when within vision range
- **Attack** - Attacks the player when within attack range, with cooldown between strikes

## Dungeon Generation

Dungeons are procedurally generated using the `DungeonSpawnSystem`:

- Rooms are placed on a grid with configurable count and seed
- Each room contains walls, floors, and decorative elements from the prefab library
- Doors connect rooms and open automatically when the player approaches
- Enemies are spawned within rooms

### Prefab Types

| Prefab | Asset | Usage |
|--------|-------|-------|
| Wall | `wall.gltf` | Room boundaries |
| WallCorner | `wall_corner.gltf` | Corner pieces |
| WallCrossing | `wall_crossing.gltf` | Four-way intersections |
| WallTsplit | `wall_Tsplit.gltf` | T-intersections |
| WallDoorway | `wall_doorway_scaffold.gltf` | Door frames |
| Door | `door.gltf` | Openable doors |
| FloorLarge | `floor_tile_large.gltf` | Main floor tiles |
| FloorSmall | `floor_tile_small.gltf` | Detail floor tiles |
| Stairs | `stairs_walled.gltf` | Level transitions |
| WallCornerSmall | `wall_corner_small.gltf` | Small corner fills |

## Menu System

### Main Menu
- **Play** - Starts a new game (transitions to GameplayScene)
- **Settings** - Opens the settings screen
- **Credits** - Shows team credits overlay
- **Quit** - Shows "Are you sure?" confirmation dialog

### Pause Menu (ESC during gameplay)

The pause menu is pushed as an overlay on top of the gameplay scene. The game world freezes (no updates) but remains visible behind a dark semi-transparent overlay. All pause/settings overlays use a scene stack so resuming returns to the exact game state.

- **Resume** - Pops the pause overlay, resuming gameplay
- **Settings** - Pushes settings overlay on top of the pause menu
- **Main Menu** - Replaces the entire scene stack with MainMenuScene
- **Quit** - Shows quit confirmation dialog

### Settings
- **Music Volume** - Slider control (0-100%)
- **VFX Volume** - Slider control (0-100%)
- **Back** - Pops the settings overlay, returning to the previous screen

Settings are stored in a **singleton** (`GameSettings`) — a class with a single global instance accessed via `GameSettings::Instance()`. This ensures that volume values persist across scene transitions. When the user adjusts the Music slider in SettingsScene, it writes directly to `GameSettings::Instance().MusicVolume`. When the scene is destroyed and recreated later, it reads from the same singleton, so the value is preserved without needing to save/load from a file.

## UI Components

The game uses two reusable UI widget classes:

- **UIButton** - Text button with hover highlighting (gold on hover, white default). Supports centering and hit-testing.
- **UISlider** - Labeled slider with track, fill, and handle. Binds directly to a `float*` in GameSettings for immediate value updates.

## Controls

| Input | Action |
|-------|--------|
| W/A/S/D or Arrow Keys | Move player |
| Space | Attack (combo chain) |
| Mouse Wheel | Camera zoom in/out |
| ESC | Pause menu (during gameplay) |
| F3 | Toggle debug mode (enables F4/F5) |
| F4 | Toggle FPS overlay (requires F3) |
| F5 | Toggle collider visualization (requires F3) |

## Team

- **Joey Driedger** - Architecture Lead
- **Adam Van Woerden** - Rendering Lead
- **Elijah Fabon** - Game Design Lead
