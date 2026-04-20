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
- The player **lunges forward** during attacks, closing distance to enemies (configurable speed and duration)
- The final hit in a combo chain applies a **knockback multiplier** (default 2.5x), sending enemies flying back further
- After being hit, entities receive **invincibility frames** (i-frames) during which they cannot take additional damage, preventing multi-hit per swing
- Enemies have their own AI-driven attack patterns with cooldown timers

## Enemy AI

Enemies use a state machine with four states:

- **Idle** - Stationary, waiting for the player to enter vision range
- **Patrol** - Walks between two waypoints (if configured)
- **Chase** - Moves toward the player when within vision range
- **Attack** - Attacks the player when within attack range, with cooldown between strikes. Enemies attack immediately upon first entering attack range without waiting for the cooldown timer.

Enemies are also subject to **knockback** — when hit, they are pushed away from the attacker for a brief duration (default 0.7s). During knockback, the enemy's AI is suspended and they cannot attack or chase. The final hit in the player's combo chain applies extra knockback force.

## Dungeon Generation

Dungeons are procedurally generated via a three-stage pipeline: `FloorGenerator` → `BuildRoomSystem::FromFloor` → `BuildRoomSystem::Build` → `DungeonSpawnSystem::SpawnDungeon`.

### Pipeline

1. **`FloorGenerator::Generate(config)`** — produces a `FloorLayout` (a 2D grid of `CellType` values).
   1. **Phase 1: Place rooms.** Random-size rectangles are placed one at a time; any new room whose AABB comes within 8 cells of an existing room is rejected. This large padding (`kPad = 8`) guarantees corridors connecting room centers can't graze a third room's wall and produce thin wall peninsulas.
   2. **Phase 2: Connect consecutive rooms.** Each room[i] is linked to room[i-1] with a single L-shaped corridor (random horizontal-first or vertical-first bend). Corridor width is `1` — required so the door detector (which looks for floor cells with exactly 2 walkable neighbors) can find room/corridor transitions.
   3. **Phase 3: Extra maze corridors.** `mazeFactor`-controlled; currently set to `0.0` in `GameplayScene` because additional corridors tended to graze third rooms and create noise.
   4. **Validate & retry.** The candidate layout is rejected if any `Empty` cell has `Floor` neighbors on opposite cardinal sides (would become a wall strip between parallel floors) or has 3+ cardinal floor neighbors (wall peninsula). Up to 50 attempts are made with re-seeded RNG; on exhaustion the last candidate is used and a warning is logged.
   5. **Mark endpoints.** First room's center becomes `Start` (player spawn), last room's center becomes `End` (stairs down).

2. **`BuildRoomSystem::FromFloor(layout)`** — upgrades a `FloorLayout` into a `MapGrid`.
   1. Copies `Floor`/`Start`/`End` cells into the output map.
   2. **Reachability prune.** Flood-fills from the `Start` cell; any walkable cell not reached is erased back to `Empty`, so walls/doors aren't spawned around geometry the player can't access.
   3. **Wall halo.** Every `Empty` cell adjacent (including diagonals) to a floor is turned into `Wall`.
   4. **Door detection.** A floor cell with exactly 2 walkable neighbors on opposite cardinal sides, adjacent to at least one room-like tile (3+ walkable neighbors), becomes `Door`.

3. **`BuildRoomSystem::Build(map, config)`** — turns the grid into a list of `SpawnInstance`s. Selects the correct wall variant (straight, corner, T-split, crossing) and rotation by inspecting each wall cell's cardinal neighbors. Places stair props for `Start`/`End` and emits a `Door` prop offset from the `WallDoorway` frame.

4. **`DungeonSpawnSystem::SpawnDungeon(roomCount, seed, mazeFactor)`** — drives the pipeline, instantiates prefabs as ECS entities, caches the generated `MapGrid`, records the world-space `Start` position for player placement, and calls `SpawnEnemies` / `SpawnButtons` to populate the level.

### Runtime Behavior

- Player spawn is set to the Start tile's world position after `SpawnDungeon` (since validation retries can relocate the first room, the player can no longer be placed at a hard-coded world coordinate).
- Doors open automatically when the player approaches; they can also be linked to floor-switch `TriggerId`s for puzzle mechanics.
- Enemies spawn only inside rooms, never in corridors.
- Press **F6** (with debug mode on via F3) to toggle a top-down map overview camera that reveals the entire generated floor — useful for validating generation.

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
| FloorLarge (variant) | `floor_tile_big_grate.gltf` | Grate-style large floor tile |
| Wall (variant) | `wall_broken.gltf` | Broken/damaged wall section |
| Wall (variant) | `wall_pillar.gltf` | Wall with pillar detail |
| Wall (variant) | `wall_shelves.gltf` | Wall with shelving |
| Wall (variant) | `wall_scaffold.gltf` | Wall with scaffolding |
| Wall (variant) | `wall_window_closed.gltf` | Wall with closed window |
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

## Audio / Sound Effects

The game uses an event-driven audio system where gameplay systems queue sound effects via an `AudioEventQueue`, which is processed at the end of each frame. This decouples audio playback from game logic.

| Sound | Trigger | Asset |
|-------|---------|-------|
| Background Music | Game start (loops) | `1 Exploration LOOP TomMusic.ogg` |
| Sword Attack | Each hit in the combo chain | `Sword Attack 1.ogg` |
| Enemy Death | Enemy health reaches zero | `Goblin_00.mp3` |
| Door Open | Player approaches a proximity door | `Door Open 1.ogg` |

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
