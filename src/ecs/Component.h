//
// Created by adamd on 2026-01-21.
//

#ifndef ASSIGNMENT1_COMPONENT_H
#define ASSIGNMENT1_COMPONENT_H
#include <functional>
#include <SDL3/SDL_render.h>
#include <string>
#include <unordered_map>
#include <vendor/glm/gtc/matrix_transform.hpp>
#include <vendor/glm/gtc/type_ptr.hpp>
#include "Mesh.h"
#include "AinmationClip.h"
#include "Animation3DClips.h"
#include "BoneInfo.h"
#include "BoneNode.h"
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

struct Animation {
    std::unordered_map<std::string, AnimationClip> clips{};
    std::string currentClip{};
    float time{}; // time accumulated for the current frame in the clip
    int currentFrame{}; // index of the current frame in the clip
    float speed = 0.1f; // time per frame
    int lastDirection = 2;
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
    float attackRange = 3.0f;
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
};

struct Collider3D
{
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
    glm::vec3 rotation; // pitch, yaw, roll in radians
    glm::vec3 scale;

    glm::vec3 oldPosition;

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
    std::vector<Mesh>* meshes;

    std::unordered_map<std::string, int> boneMap;
    std::vector<BoneInfo> boneInfo;
    int boneCounter = 0;

    std::vector<BoneNode> skeleton; // runtime skeleton
    int rootBoneIndex;         // index of hips

    Model() = default;
    Model(Model* model) :
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
    float currentTime = 0.0f;
    int currentClip = 0;

    std::vector<glm::mat4> finalBoneMatrices;

    Animator() : finalBoneMatrices(100, glm::mat4(1.0f)) {};
};

struct PlayerTag{};
struct ProjectileTag {};

#endif //ASSIGNMENT1_COMPONENT_H