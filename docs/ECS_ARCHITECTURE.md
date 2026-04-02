# ECS Architecture

## Overview

The engine uses an Entity-Component-System (ECS) pattern where:

- **Entities** are lightweight integer handles (no data or behavior)
- **Components** are plain data structs attached to entities
- **Systems** are stateless (or minimal-state) functions that operate on entities with specific component combinations

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

A `std::bitset<64>` where each bit corresponds to a registered component type. Every entity has a signature that tracks which components are attached to it. Systems can use signatures to filter entities.

### ComponentStorage (`engine/ecs/ComponentStorage.h`)

A generic, contiguous storage container for a single component type.

**Design**: Uses parallel vectors and an entity-to-index map for O(1) access:
```
m_Components: [TransformA, TransformB, TransformC, ...]
m_Entities:   [Entity(3),  Entity(1),  Entity(7),  ...]
m_EntityToIndex: { 3->0, 1->1, 7->2, ... }
```

**Removal**: Uses "swap and pop" - the removed element is swapped with the last element, then the vector is shrunk. This keeps storage contiguous with no gaps.

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

All game components are defined in `game/include/game/components/`. They are plain structs with public members.

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
Empty marker component. Identifies the player entity for systems that need to find it.

### Health
Entity health for combat.
```
Current  - Current hit points (default 100)
Maximum  - Maximum hit points (default 100)
```

### Collider
Axis-aligned bounding box for collision detection.
```
Width, Height, Depth       - Box dimensions
OffsetX, OffsetY, OffsetZ  - Offset from Transform position
IsTrigger                  - If true, detects overlap but doesn't block movement
IsStatic                   - If true, doesn't get pushed by collisions
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
Attaches a child entity to a parent entity's skeleton bone.
```
ParentEntity  - Entity with the AnimationState
BoneName      - Name of the bone to attach to (e.g., "handslot.r")
Offset        - mat4 offset relative to the bone
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

All game systems are in `game/include/game/systems/`. Systems are either static classes (stateless, called via `SystemName::Update(...)`) or instance-based (hold state, called via `m_system.Update(...)`).

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
| **HealthSystem** | Defined | Monitors health values, handles death state |

### World Systems

| System | Style | Purpose |
|--------|-------|---------|
| **DungeonSpawnSystem** | Instance | Procedurally generates dungeon rooms with walls, floors, doors, enemies |
| **EntitySpawnSystem** | Static | Registers component types and provides `SpawnPlayer()` prefab function |
| **DoorSystem** | Static | Animates door swing based on `Door::Open` state |
| **DoorPuzzleSystem** | Static | Links `Switch` presses to `Door` opens via matching IDs |
| **SwitchTriggerSystem** | Static | Detects `Player` collisions with `Switch` entities, sets `Pressed = true` |
| **RoomTransitionSystem** | Defined | Manages loading/unloading of room contents |
| **LifetimeSystem** | Static | Decrements `Lifetime::Duration`, destroys entities when expired |
| **AudioSystem** | Defined | Audio playback management |

### UI & Debug Systems

| System | Style | Purpose |
|--------|-------|---------|
| **UISystem** | Instance | Renders HUD elements (health bar) using TextRenderer and QuadRenderer |
| **DebugToggle** | Instance | Toggles debug visualizations via keyboard shortcuts |
| **DebugColliderRenderer** | Instance | Draws wireframe collider boxes in 3D space |
| **FPSCounter** | Instance | Tracks and displays frames per second in the window title |
| **EnemyAISystem** | Instance | Runs AI state machine: Idle → Patrol → Chase → Attack |

## System Execution Order

Systems are executed in a specific order each frame in `GameplayScene::OnUpdate()`:

```
1. CombatInputSystem    (read player input)
2. MovementSystem       (apply movement)
3. CollisionSystem      (resolve overlaps)
4. AnimationSystem      (update skeletons)
5. BoneAttachmentSystem (position attached items)
6. AttackHitboxSystem   (track hitboxes)
7. EnemyAISystem        (run AI behavior)
8. CombatSystem         (process attacks)
9. DamageSystem         (apply damage)
10. SwitchTriggerSystem (check switches)
11. DoorSystem          (animate doors)
12. DoorPuzzleSystem    (link switches to doors)
13. LifetimeSystem      (destroy expired entities)
14. CameraFollowSystem  (update camera)
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
