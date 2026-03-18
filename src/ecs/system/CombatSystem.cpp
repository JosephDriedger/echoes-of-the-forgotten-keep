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

void CombatSystem::spawnProjectile(Entity* attacker, Vector2D direction, std::string tag)
{
    if (direction.length() == 0)
        return;

    auto& transform = attacker->getComponent<Transform>();

    auto& projectile = world.createDeferredEntity();

    auto& t = projectile.addComponent<Transform>();
    t.position = transform.position;
    t.scale = 1;

    auto& v = projectile.addComponent<Velocity>();
    v.direction = direction.normalized();
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