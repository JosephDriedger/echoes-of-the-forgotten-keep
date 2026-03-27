//
// Created by adamd on 2026-01-28.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_KEYBOARDINPUTSYSTEM_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_KEYBOARDINPUTSYSTEM_H
#include <memory>
#include <vector>
#include <SDL3/SDL_events.h>

#include "../Component.h"
#include "../Entity.h"

class KeyboardInputSystem {
public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, const SDL_Event& event) {
        const bool *keyState = SDL_GetKeyboardState(NULL);

        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>() && e->hasComponent<Velocity3D>() &&
                e->hasComponent<Animator>() && e->hasComponent<Animation3D>()) {
                auto& v = e->getComponent<Velocity3D>();
                auto& a = e->getComponent<Animator>();
                auto& anim = e->getComponent<Animation3D>();

                if (a.isDead)
                    continue;

                v.direction = glm::vec3(0.0f);

                if (keyState[SDL_SCANCODE_W]) v.direction.z -= 1;
                if (keyState[SDL_SCANCODE_S]) v.direction.z += 1;
                if (keyState[SDL_SCANCODE_A]) v.direction.x -= 1;
                if (keyState[SDL_SCANCODE_D]) v.direction.x += 1;

                if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                        // case SDLK_W:
                        //     //up
                        //     v.direction.z = -1;
                        //     break;
                        // case SDLK_S:
                        //     v.direction.z = 1;
                        //     break;
                        // case SDLK_A:
                        //     v.direction.x = -1;
                        //     break;
                        // case SDLK_D:
                        //     v.direction.x = 1;
                        //     break;
                        case SDLK_LCTRL:
                            if (a.isDead)
                                break;

                            a.health -= 1;

                            // 🔥 INTERRUPT EVERYTHING
                            a.isAttacking = false;
                            a.attackQueued = false;
                            a.comboIndex = 0;

                            if (a.health <= 0)
                            {
                                std::cout << "Died" << std::endl;
                                v.direction = glm::vec3(0.0f);
                                a.isDead = true;
                                a.currentTime = 0.0f;
                            }
                            else
                            {
                                std::cout << "health: " << a.health << std::endl;
                                v.direction = glm::vec3(0.0f);
                                a.isHit = true;
                                a.hitTimer = 0.0f;
                                a.currentTime = 0.0f;
                            }
                            break;
                        case SDLK_SPACE:
                            if (a.isAttacking) {
                                // queue combo if timing is right
                                if (a.comboTimer > 0.0f && a.comboTimer <= a.comboWindow) {
                                    a.attackQueued = true;
                                }
                            } else {
                                a.isAttacking = true;
                                a.comboIndex = 0;
                                a.currentTime = 0.0f;
                            }
                        {
                            auto& transform = e->getComponent<Transform3D>();
                            glm::vec3 forward;
                            forward.x = sin(glm::radians(transform.rotation.y));
                            forward.z = cos(glm::radians(transform.rotation.y));
                            forward.y = 0.0f;
                            forward = glm::normalize(forward);
                            glm::vec3 dir = forward;
                            auto& req = e->addComponent<AttackRequest>();
                            req.direction = dir;
                            req.tag = "projectile";
                        }
                            break;
                        case SDLK_RIGHT:
                            a.currentClip++;
                            if (a.currentClip >= anim.clips->size())
                                a.currentClip = 0;
                            a.currentTime = 0.0f;
                            std::cout << "Switched to animation: "
                                << anim.clips->at(a.currentClip).name
                                << " (" << a.currentClip << ")\n";
                            break;
                        case SDLK_LEFT:
                            a.currentClip--;
                            if (a.currentClip <= 0)
                                a.currentClip = anim.clips->size()-1;
                            a.currentTime = 0.0f;
                            std::cout << "Switched to animation: "
                                << anim.clips->at(a.currentClip).name
                                << " (" << a.currentClip << ")\n";
                            break;
                        default:
                            break;

                    }
                }
                else if (event.type == SDL_EVENT_KEY_UP) {
                    // switch (event.key.key) {
                    //     case SDLK_W:
                    //         //up
                    //         v.direction.z = 0;
                    //         break;
                    //     case SDLK_S:
                    //         v.direction.z = 0;
                    //         break;
                    //     case SDLK_A:
                    //         v.direction.x = 0;
                    //         break;
                    //     case SDLK_D:
                    //         v.direction.x = 0;
                    //         break;
                    //     default:
                    //         break;
                    // }
                }

            }
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_KEYBOARDINPUTSYSTEM_H