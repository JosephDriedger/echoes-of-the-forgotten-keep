//
// Created by scept on 2026-03-11.
//

#include "../../include/game/systems/CombatSystem.h"
#include "World.h"

void CombatSystem::update(std::vector<std::unique_ptr<Entity>>& entities, float dt)
{
    for (auto& e : entities)
    {
        // if (e->hasComponent<Combat>()) {
        //     auto& hitbox = e->getComponent<Combat>().weapon->getComponent<ProjectileTag>();
        //     if (hitbox.melee) {
        //         if (hitbox.active) {
        //             hitbox.activationTime -= dt;
        //             if (hitbox.activationTime <= 0) {
        //                 hitbox.active = false;
        //                 std::cout << "Hitbox Inactive" << std::endl;;
        //             }
        //         }
        //     }
        // }

        if (!e->hasComponent<AttackRequest>()) {continue;}

        auto& req = e->getComponent<AttackRequest>();

        spawnProjectile(e.get(), req.direction, req.tag);
        // if (e->hasComponent<Combat>()) {
        //     auto& weapon = e->getComponent<Combat>().weapon;
        //     if (weapon->hasComponent<ProjectileTag>()) {
        //         auto& hitbox = weapon->getComponent<ProjectileTag>();
        //         if (hitbox.melee && !hitbox.active) {
        //             hitbox.active = true;
        //             hitbox.activationTime = 1.0f;
        //             std::cout << "Hitbox Active" << std::endl;
        //         }
        //     }
        // }

        e->removeComponent<AttackRequest>();
    }
}

void CombatSystem::spawnProjectile(Entity* attacker, glm::vec3 direction, std::string tag)
{
    if (direction.length() == 0)
        return;

    auto& transform = attacker->getComponent<Transform3D>();

    auto& projectile = world.createDeferredEntity();

    auto& t = projectile.addComponent<Transform3D>();
    t.scale = glm::vec3(1.0);

    float spawnDistance = 1.3f; // tweak this

    glm::vec3 forward = glm::normalize(direction);
    t.position = transform.position + forward * spawnDistance;

    auto& v = projectile.addComponent<Velocity3D>();
    v.direction = glm::normalize(direction);
    v.speed = 0;

    auto& c = projectile.addComponent<Collider3D>(0.5, 0.5, 0.5);
    c.tag = tag;
    // c.rect = {t.position.x, t.position.y, 16, 16};

    projectile.addComponent<ProjectileTag>(0.5);
    projectile.addComponent<Damage>().amount = 1;
    projectile.addComponent<Parent>().entity = attacker;

    // SDL_Texture* tex = TextureManager::load("../asset/ball.png");
    // SDL_FRect src {0, 0, 32, 32};
    // SDL_FRect dest {t.position.x, t.position.y, 32, 32};
    // projectile.addComponent<Sprite>(tex, src, dest);
}