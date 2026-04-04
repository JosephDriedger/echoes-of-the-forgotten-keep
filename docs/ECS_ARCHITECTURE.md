# ECS Architecture

## Overview

The engine uses an Entity-Component-System (ECS) pattern, a data-oriented design that separates identity, data, and logic:

- **Entities** are lightweight integer handles — just a number, no data or behavior. Think of an entity as a row ID in a database.
- **Components** are plain data structs attached to entities — they hold state but contain no logic. Think of components as columns in a database.
- **Systems** are functions that operate on entities with specific component combinations — they contain all the logic but hold no per-entity state.

This separation means that adding new behavior (e.g., making doors openable) only requires adding a new component (Door data) and a new system (DoorSystem logic), without modifying any existing entity or component classes.

All ECS state is managed by the **Registry**, which serves as the central coordinator.

## Core ECS Classes

### Entity (`engine/ecs/Entity.h`)

An entity is a thin wrapper around a `uint32_t` identifier.

- **MAX_ENTITIES**: 5000
- **INVALID_ENTITY_ID**: 0 (reserved, never assigned)
- Supports comparison operators and validity checking via `IsValid()`

### Component (`engine/ecs/Component.h`)

Components are identified by a `ComponentType` (`uint8_t`), assigned at registration time.

- **MAX_COMPONENTS**: 64 (determines bitset width for signatures)
- Components are registered with the Registry via `RegisterComponent<T>()`

### Signature (`engine/ecs/Signature.h`)

A `std::bitset<64>` where each bit position corresponds to a registered component type. For example, if `Transform` is component type 0 and `Health` is component type 3, an entity with both would have bits 0 and 3 set: `...001001`. Every entity has a signature that tracks which components are attached to it. When a component is added or removed, the corresponding bit is flipped. This allows quick checks like "does this entity have a Transform and a Health component?" by comparing bit patterns.

### ComponentStorage (`engine/ecs/ComponentStorage.h`)

A generic, contiguous storage container for a single component type.

**Design**: Uses parallel vectors and an entity-to-index map for O(1) access:
```
m_Components: [TransformA, TransformB, TransformC, ...]
m_Entities:   [Entity(3),  Entity(1),  Entity(7),  ...]
m_EntityToIndex: { 3->0, 1->1, 7->2, ... }
```

**Removal**: Uses "swap and pop" — when an element is removed from the middle, it is swapped with the last element in the vector, then the vector size is decremented by one. This avoids shifting all subsequent elements (which would be O(n)) and keeps the storage contiguous with no gaps. The trade-off is that element order is not preserved, but since systems iterate all entities anyway, order does not matter.

### Registry (`engine/ecs/Registry.h`)

The central ECS coordinator. All entity and component operations go through the Registry.

**Entity Management:**
- `CreateEntity()` - Pops an ID from the available stack, marks it alive
- `DestroyEntity(entity)` - Removes all components, clears signature, pushes ID back
- `DestroyAllEntities()` - Bulk cleanup
- `GetActiveEntities()` - Returns vector of all living entities
- `IsAlive(entity)` - Check if an entity exists

**Component Management:**
- `RegisterComponent<T>()` - Assigns a ComponentType ID and creates storage
- `AddComponent<T>(entity, component)` - Copy-adds a component
- `EmplaceComponent<T>(entity, args...)` - Constructs in-place
- `RemoveComponent<T>(entity)` - Removes and updates signature
- `HasComponent<T>(entity)` - Checks via storage lookup
- `GetComponent<T>(entity)` - Returns reference to component data

**Signature Management:**
- `SetSignature()` / `GetSignature()` / `ClearSignature()` - Direct signature access

**Lifecycle:**
- `Reset()` - Destroys all entities and clears registrations
- `ClearComponentRegistrations()` - Removes all component type mappings

### System (`engine/ecs/System.h`)

Base class providing a `std::set<Entity> m_Entities` member for systems that need to track their matching entities. In practice, most game systems iterate `Registry::GetActiveEntities()` and check for required components directly.

## Components

All game components are defined in a single header: `game/include/game/components/Components.h`. They are plain structs with public members.

### Transform
Position, rotation, and scale in 3D space.
```
X, Y, Z              - World position
RotationY             - Y-axis rotation (radians)
ScaleX, ScaleY, ScaleZ - Per-axis scale
ModelMatrix           - Optional pre-computed model matrix
UseModelMatrix        - If true, use ModelMatrix instead of computing from pos/rot/scale
```

### Render
Visual representation linking to GPU resources.
```
MeshPtr     - shared_ptr<Mesh> (vertex data on GPU)
TexturePtr  - shared_ptr<Texture> (diffuse texture)
```

### Player
Empty **marker component** — a struct with no data members. Its sole purpose is to tag an entity as "the player" so that systems can distinguish it from enemies and world objects. For example, `MovementSystem` iterates all entities but only moves ones that have a `Player` component. This is a common ECS pattern for filtering entities by role without using inheritance.

### Health
Entity health for combat.
```
Current  - Current hit points (default 100)
Maximum  - Maximum hit points (default 100)
```

### Collider
Axis-Aligned Bounding Box (AABB) for collision detection. An AABB is a box whose edges are aligned with the world axes (X, Y, Z) — it cannot rotate. This makes overlap checks very fast (just compare min/max coordinates on each axis) at the cost of less precise shapes for rotated objects.
```
Width, Height, Depth       - Box dimensions
OffsetX, OffsetY, OffsetZ  - Offset from Transform position (e.g., to center the box on the entity)
IsTrigger                  - If true, detects overlap but doesn't block movement (used for floor switches)
IsStatic                   - If true, doesn't get pushed by collisions (used for walls and doors)
```

### AnimationState
Skeletal animation playback state.
```
SkeletonPtr         - Shared pointer to bone hierarchy
Clips               - Shared pointer to animation clip library
CurrentClip         - Index of the currently playing clip
CurrentTime         - Playback position within the clip
BlendTime           - Duration of cross-fade between clips
FinalBoneMatrices   - Array of 100 mat4 values uploaded to the vertex shader
AnimState           - Current state (Idle, Running, Attacking, Hit, Dead)
IdleClipIndex, RunClipIndex, Attack1-3ClipIndex, HitClipIndex, DeathClipIndex
```

### CombatState
Combat mechanics and combo system.
```
AttackDamage     - Damage per hit
IsAttacking      - Currently in an attack animation
AttackQueued     - Player queued a follow-up attack
ComboWindowOpen  - Combo input window is active
ComboIndex       - Current position in combo chain (0-2)
ComboWindow      - Duration of the combo input window
IncomingHit      - Optional pending damage from an enemy
IsDead           - Entity has been killed
```

### EnemyAI
Enemy behavior state machine.
```
MoveSpeed       - Movement speed
State           - Current AI state (Idle, Patrol, Chase, Attack)
VisionRange     - Distance at which the enemy detects the player
AttackRange     - Distance at which the enemy attacks
AttackCooldown  - Time between attacks
PatrolA, PatrolB - Waypoint positions for patrol behavior
HasPatrol       - Whether this enemy patrols
```

### Door
Door state and swing animation.
```
BaseRotationY    - Rotation when fully closed
CurrentAngle     - Current swing angle
SwingSpeed       - Degrees per second
SwingDirection   - +1 or -1
TriggerId        - ID of the switch that opens this door
Open             - Whether the door should be open
TargetAngle      - Angle the door is animating toward
OpenAngle, CloseAngle - Angle limits
ColliderEntity   - Entity with the door's collider
PanelLength, PanelThickness - Physical dimensions for collider updates
```

### Room
```
Id  - Which room this entity belongs to
```

### BoneAttachment
Attaches a child entity's Transform to a bone in a parent entity's skeleton, so the child moves with the bone each frame. This is how the sword and shield follow the knight's hands during animations — the sword entity has a `BoneAttachment` pointing to the right hand bone (`"handslot.r"`), and `BoneAttachmentSystem` copies the bone's world-space transform to the sword's `Transform` every frame.
```
ParentEntity  - Entity with the AnimationState (e.g., the knight)
BoneName      - Name of the bone to attach to (e.g., "handslot.r")
Offset        - mat4 offset relative to the bone (adjusts position/rotation)
```

### Switch
Trigger mechanism for puzzles and doors.
```
Id       - String identifier matching a Door's TriggerId
Pressed  - Whether the switch has been activated
```

### Lifetime
Automatic entity destruction after a duration.
```
Duration  - Seconds remaining (default 5.0)
```

## Systems

All game systems are in `game/include/game/systems/`. Systems come in two styles:

- **Static** systems have no member variables. All their methods are `static`, so they are called directly on the class (e.g., `MovementSystem::Update(registry, input, dt)`). This is used when the system needs no persistent state between frames — it simply reads components, does work, and writes results back.
- **Instance** systems are created as member variables of GameplayScene (e.g., `m_CollisionSystem`). They hold state that persists across frames, such as cached data structures, tracked entity sets, or GPU resources. They are called on the instance (e.g., `m_CollisionSystem.Update(registry)`).

### Core Gameplay Systems

| System | Style | Purpose |
|--------|-------|---------|
| **MovementSystem** | Static | Reads keyboard input (WASD), moves entities with `Player` + `Transform` components, updates facing direction |
| **CollisionSystem** | Instance | AABB collision detection between all `Collider` + `Transform` entities, resolves overlaps for non-static entities |
| **CameraFollowSystem** | Instance | Smoothly follows the player entity, handles mouse wheel zoom (range 8-60) |
| **AnimationSystem** | Static | Advances animation time, evaluates bone keyframes, writes `FinalBoneMatrices` |
| **BoneAttachmentSystem** | Static | Updates `Transform` of entities with `BoneAttachment` to follow parent bones (e.g., sword in hand) |

### Combat Systems

| System | Style | Purpose |
|--------|-------|---------|
| **CombatInputSystem** | Static | Translates mouse clicks into `CombatState` attack requests, manages combo queuing |
| **CombatSystem** | Instance | Processes attack timing, combo windows, animation state transitions |
| **AttackHitboxSystem** | Instance | Tracks active attack hitboxes, prevents multi-hit per swing |
| **DamageSystem** | Instance | Processes `IncomingHit` on `CombatState`, applies damage to `Health` |
| **DeathSystem** | Static | Marks dead entities (Health <= 0) with `IsDead`, clears movement, adds corpse `Lifetime` |

### AI Systems

| System | Style | Purpose |
|--------|-------|---------|
| **EnemyAISystem** | Instance | Runs AI state machine: Idle / Patrol / Chase / Attack (skips dead entities) |

### World Systems

| System | Style | Purpose |
|--------|-------|---------|
| **DungeonSpawnSystem** | Instance | Procedurally generates dungeon rooms with walls, floors, doors, enemies |
| **EntitySpawnSystem** | Static | Registers component types and provides `SpawnPlayer()` prefab function |
| **DoorSystem** | Static | Proximity-based door opening (detects player, sets swing direction) |
| **DoorPuzzleSystem** | Static | Links `Switch` presses to `Door` opens via matching trigger IDs |
| **DoorAnimationHelper** | Static | Shared door swing animation and collider update (used by DoorSystem and DoorPuzzleSystem) |
| **SwitchTriggerSystem** | Static | Detects `Player` collisions with `Switch` entities, sets `Pressed = true` |
| **RoomTransitionSystem** | Defined | Manages loading/unloading of room contents |
| **LifetimeSystem** | Static | Decrements `Lifetime::Duration`, destroys entities when expired |

### UI & Debug Systems

| System | Style | Purpose |
|--------|-------|---------|
| **UISystem** | Instance | Renders HUD elements (health bar) |
| **DebugToggle** | Instance | Toggles debug visualizations via F3/F4/F5 |
| **DebugColliderRenderer** | Instance | Draws wireframe collider boxes in 3D space |
| **FPSCounter** | Instance | Tracks and displays FPS as on-screen overlay |

## System Execution Order

Systems are executed in a specific order each frame in `GameplayScene::OnUpdate()`:

```
1. CombatInputSystem    (read player input)
2. MovementSystem       (apply movement)
3. CollisionSystem      (resolve overlaps)
4. AnimationSystem      (update skeletons)
5. BoneAttachmentSystem (position attached items)
6. AttackHitboxSystem   (track hitboxes)
7. DamageSystem         (apply damage)
8. DeathSystem          (mark dead entities, add corpse timer)
9. EnemyAISystem        (run AI behavior, skip dead)
10. CombatSystem        (process attacks)
11. SwitchTriggerSystem (check switches)
12. DoorSystem          (proximity doors)
13. DoorPuzzleSystem    (trigger doors)
14. LifetimeSystem      (destroy expired entities)
15. CameraFollowSystem  (update camera)
```

This ordering ensures that input is processed first, physics resolves before rendering, and the camera captures the final state.

## Usage Example

Creating an entity with components:

```cpp
// Register component types (done once at startup)
EntitySpawnSystem::Register(registry);  // registers Transform, Render, Player, etc.

// Create an entity
engine::Entity entity = registry.CreateEntity();

// Add components
registry.AddComponent(entity, Transform(10.0f, 0.0f, 5.0f));
registry.AddComponent(entity, Render(meshPtr, texturePtr));
registry.AddComponent(entity, Health(100, 100));
registry.AddComponent(entity, Collider());
registry.EmplaceComponent<Player>(entity);  // marker component, no data

// Query in a system
for (const engine::Entity e : registry.GetActiveEntities())
{
    if (!registry.HasComponent<Transform>(e) || !registry.HasComponent<Health>(e))
        continue;

    auto& transform = registry.GetComponent<Transform>(e);
    auto& health = registry.GetComponent<Health>(e);
    // ... system logic
}
```
