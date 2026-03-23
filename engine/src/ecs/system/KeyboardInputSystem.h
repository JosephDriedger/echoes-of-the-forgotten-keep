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
        for (auto& e : entities) {
            if (e->hasComponent<PlayerTag>() && e->hasComponent<Velocity3D>() &&
                e->hasComponent<Animator>() && e->hasComponent<Animation3D>()) {
                auto& v = e->getComponent<Velocity3D>();
                auto& a = e->getComponent<Animator>();
                auto& anim = e->getComponent<Animation3D>();
                if (event.type == SDL_EVENT_KEY_DOWN) {
                    switch (event.key.key) {
                        case SDLK_W:
                            //up
                            v.direction.z = -1;
                            break;
                        case SDLK_S:
                            v.direction.z = 1;
                            break;
                        case SDLK_A:
                            v.direction.x = -1;
                            break;
                        case SDLK_D:
                            v.direction.x = 1;
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
                if (event.type == SDL_EVENT_KEY_UP) {
                    switch (event.key.key) {
                        case SDLK_W:
                            //up
                            v.direction.z = 0;
                            break;
                        case SDLK_S:
                            v.direction.z = 0;
                            break;
                        case SDLK_A:
                            v.direction.x = 0;
                            break;
                        case SDLK_D:
                            v.direction.x = 0;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_KEYBOARDINPUTSYSTEM_H