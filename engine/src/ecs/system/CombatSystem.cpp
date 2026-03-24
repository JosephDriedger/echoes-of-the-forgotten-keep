//
// Created by scept on 2026-03-11.
//

#include "CombatSystem.h"
#include "World.h"

void CombatSystem::update(std::vector<std::unique_ptr<Entity>>& entities)
{
    for (auto& e : entities)
    {
        if (!e->hasComponent<AttackRequest>()) {continue;}

        auto& req = e->getComponent<AttackRequest>();

        spawnProjectile(e.get(), req.direction, req.tag);

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
    t.position = transform.position;
    t.scale = glm::vec3(1.0);

    auto& v = projectile.addComponent<Velocity3D>();
    v.direction = glm::normalize(direction);
    v.speed = 1000;

    auto& c = projectile.addComponent<Collider>();
    c.tag = tag;
    c.rect = {t.position.x, t.position.y, 16, 16};

    projectile.addComponent<ProjectileTag>();
    projectile.addComponent<Damage>().amount = 1;
    projectile.addComponent<Parent>().entity = attacker;

    SDL_Texture* tex = TextureManager::load("../asset/ball.png");
    SDL_FRect src {0, 0, 32, 32};
    SDL_FRect dest {t.position.x, t.position.y, 32, 32};
    projectile.addComponent<Sprite>(tex, src, dest);
}