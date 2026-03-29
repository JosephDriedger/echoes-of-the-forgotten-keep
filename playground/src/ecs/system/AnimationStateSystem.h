//
// Created by adamd on 2026-03-23.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSTATESYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSTATESYSTEM_H
#include <memory>
#include <vector>

#include "Component.h"
#include "Entity.h"

class AnimationStateSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& e : entities) {
            if (e->hasComponent<Velocity3D>() && e->hasComponent<Animator>()) {

                auto& v = e->getComponent<Velocity3D>();
                auto& a = e->getComponent<Animator>();

                float speed = glm::length(v.direction);

                a.previousState = a.currentState;

                if (a.isDead) {
                    a.currentState = AnimState::Death;
                    a.corpseTimer -= dt;
                    if (a.corpseTimer <= 0.0f) {
                        e->destory();
                    }
                }
                else if (a.isHit) {
                    a.currentState = AnimState::HitReact;
                }
                else if (a.isAttacking) {
                    a.currentState = static_cast<AnimState>(
                        static_cast<int>(AnimState::Attack1) + a.comboIndex
                    );
                }
                else {
                    speed = glm::length(v.direction);

                    if (speed < 0.01f)
                        a.currentState = AnimState::Idle;
                    else
                        a.currentState = AnimState::Run;
                }

                if (a.currentState != a.previousState) {
                    a.currentTime = 0.0f;
                    a.nextTime = 0.0f;
                    a.blendTime = 0.0f;
                    a.isBlending = true;

                    switch (a.currentState) {
                        case AnimState::Idle:
                            a.nextClip = 7;
                            break;
                        case AnimState::Run:
                            a.nextClip = 21;
                            break;
                        case AnimState::Attack1:
                            a.nextClip = 97;
                            break;
                        case AnimState::Attack2:
                            a.nextClip = 99;
                            break;
                        case AnimState::Attack3:
                            a.nextClip = 98;
                            break;
                        case AnimState::HitReact:
                            a.nextClip = 5;
                            break;
                        case AnimState::Death:
                            a.nextClip = 1;
                            break;
                        default: ;
                    }
                }
            }
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_ANIMATIONSTATESYSTEM_H