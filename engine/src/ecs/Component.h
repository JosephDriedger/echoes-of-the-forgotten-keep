//
// Created by adamd on 2026-01-21.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H
#include <functional>
#include <SDL3/SDL_render.h>
#include <string>
#include <unordered_map>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/glm/gtc/type_ptr.hpp>
#include "Mesh.h"
#include "AnimationKey.h"
#include "BoneInfo.h"
#include "Entity.h"
#include "TextureManager.h"
#include "Vector2D.h"

struct Transform {
    Vector2D position{};
    float rotation{};
    float scale{};
    Vector2D oldPosition{};
};

// direction and speed
struct Velocity {
    Vector2D direction{};
    float speed{};
};

enum class RenderLayer {
    World,
    UI
};

struct Sprite {
    SDL_Texture *texture = nullptr;
    SDL_FRect src{};
    SDL_FRect dst{};
    RenderLayer renderLayer = RenderLayer::World;
    bool visible = true;
};

struct Clickable {
    std::function<void()> onPressed{};
    std::function<void()> onReleased{};
    std::function<void()> onCancel{};
    bool pressed = false;
};

struct Collider {
    std::string tag;
    SDL_FRect rect{};
    bool enabled = true;
};

struct Camera {
    SDL_FRect view;
    float worldWidth;
    float worldHeight;
};

struct TimedSpawner {
    float spawnInterval {};
    std::function<void()> spawnCallback {};
    float timer {};
};

// our game state, might have multiple scenes
struct SceneState {
    int coinsCollected = 0;
};

// Combat

struct Health {
    int currentHealth{};
};

enum class AIState {
    Idle,
    Patrol,
    Chase,
    Attack
};

struct AI {
    AIState state = AIState::Idle;
    float stateTimer = 0.0f;

    float visionRange = 200.0f;

    Entity* target = nullptr;

    float maxChaseTime = 5.0f;
    float loseTargetCooldown = 2.0f;
    float loseTargetTimer = 0.0f;
};

struct Patrol {
    // Vector2D pointA{};
    // Vector2D pointB{};
    glm::vec3 pointA{};
    glm::vec3 pointB{};
    bool movingToB = true;
};

struct Attack {
    float cooldown = 3.0f;
    float timer = 0.0f;
};

struct AttackRequest {
    // Vector2D direction{};
    glm::vec3 direction{};
    std::string tag;
};

struct Combat {
    Attack attack;
    float attackRange = 1.0f;
    Entity* weapon = nullptr;
};

struct Damage {
    int amount = 1;
};

struct DamageEvent {
    Entity* target = nullptr;
    int amount = 1;
};

struct Parent {
    Entity* entity = nullptr;
};

struct Children {
    std::vector<Entity*> children{};
};

struct EnemyTag{};

// 3D Components

struct Velocity3D {
    glm::vec3 direction{0.0f};
    float speed{0.0f};
    float chase{0.0f};
};

struct Collider3D {
    std::string tag;
    float Width;
    float Height;
    float Depth;
    bool IsTrigger;

    Collider3D()
        : Width(1.0f), Height(1.0f), Depth(1.0f), IsTrigger(false)
    {
    }

    Collider3D(const float width, const float height, const float depth, const bool isTrigger = false)
        : Width(width), Height(height), Depth(depth), IsTrigger(isTrigger)
    {
    }

    Collider3D(const float width, const float height)
        : Width(width), Height(height), Depth(1.0f), IsTrigger(false)
    {
    }
};

// Transform3D tested
struct Transform3D {
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

    float rotationY = 0.0f; // current yaw
    glm::vec3 oldPosition;

    glm::mat4 modelMatrix = glm::mat4(1.0f);
    bool useMatrix = false; // 🔥 NEW

    Transform3D(glm::vec3 pos = glm::vec3(0.0f),
                glm::vec3 rot = glm::vec3(0.0f),
                glm::vec3 scl = glm::vec3(1.0f))
        : position(pos), rotation(rot), scale(scl) {}
};

// Camera3D part tested
struct Camera3D {
    glm::vec3 forward = glm::vec3(0,0,-1);
    glm::vec3 up      = glm::vec3(0,1,0);
    float fov         = 45.0f;
    float aspectRatio = 800.0f / 600.0f;
    float nearPlane   = 0.1f;
    float farPlane    = 100.0f;
};

// Model tested
struct Model {
    std::vector<Mesh>* meshes {};

    std::unordered_map<std::string, int> boneMap;
    std::vector<BoneInfo> boneInfo;
    int boneCounter = 0;

    std::vector<BoneNode> skeleton;
    int rootBoneIndex = 0;

    Model() = default;
    Model(const Model* model) :
        meshes(model->meshes),
        boneMap(model->boneMap),
        boneInfo(model->boneInfo),
        boneCounter(model->boneCounter),
        skeleton(model->skeleton),
        rootBoneIndex(model->rootBoneIndex) {};
};

// Texture3D tested
struct Texture3D {
    GLuint id;

    Texture3D(GLuint id) : id(id) {};
};

struct Animation3D
{
    std::vector<Animation3DClip>* clips;

    Animation3D() = default;

    Animation3D(std::vector<Animation3DClip>* clips) : clips(clips) {};
};

struct Animator
{
    int currentClip = 0;
    int nextClip = -1;

    double currentTime = 0.0f;
    double nextTime = 0.0f;

    double blendTime = 0.0f;
    double blendDuration = 0.2f;

    bool isBlending = false;

    bool isAttacking = false;
    bool attackQueued = false;
    bool comboWindowOpen = false;

    int comboIndex = 0;
    float comboWindow = 0.5f;
    float comboTimer = 0.0f;

    bool isHit = false;
    bool isDead = false;
    int health = 3;

    float hitTimer = 0.0f;

    AnimState currentState = AnimState::Idle;
    AnimState previousState = AnimState::Idle;

    std::vector<glm::mat4> finalBoneMatrices;
    std::unordered_map<std::string, glm::mat4> finalNodeTransforms;

    Animator() : finalBoneMatrices(100, glm::mat4(1.0f)) {};
};

struct BoneAttachment
{
    Entity* parent = nullptr;
    std::string boneName;
    glm::mat4 offset = glm::mat4(1.0f);
};

struct PlayerTag {};
struct ProjectileTag
{
    float duration = 5.0;
};

struct Switch {
    Entity* door = nullptr;
    bool pressed = false;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_COMPONENT_H