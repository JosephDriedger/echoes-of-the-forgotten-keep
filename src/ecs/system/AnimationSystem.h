//
// Created by adamd on 2026-02-18.
//

#ifndef ASSIGNMENT1_ANIMATIONSYSTEM_H
#define ASSIGNMENT1_ANIMATIONSYSTEM_H
#include <memory>
#include <vector>

#include "AinmationClip.h"
#include "Component.h"
#include "Entity.h"

// State System: Deciding witch clip to use
// Check if the animation has been switched
// playback system: advances the animation

class AnimationSystem {
    public:
    void update(const std::vector<std::unique_ptr<Entity>>& entities, float dt) {
        for (auto& e : entities) {
            if (e->hasComponent<Animation>() && e->hasComponent<Velocity>()) {
                auto& anim = e->getComponent<Animation>();
                auto& velocity = e->getComponent<Velocity>();

                // State system
                std::string newClip;

                anim.speed = 0.1f;
                if (e->hasComponent<PlayerTag>()){
                    if (velocity.direction.x > 0.0f) {
                        newClip = "walk_right";
                        anim.lastDirection = 1;
                    } else if (velocity.direction.x < 0.0f) {
                        newClip = "walk_left";
                        anim.lastDirection = 3;
                    } else if (velocity.direction.y > 0.0f) {
                        newClip = "walk_down";
                        anim.lastDirection = 2;
                    } else if (velocity.direction.y < 0.0f) {
                        newClip = "walk_up";
                        anim.lastDirection = 0;
                    } else {
                        // idle
                        if (anim.lastDirection == 0) {
                            newClip = "idle_up";
                            anim.speed = 0.3f;
                        } else if (anim.lastDirection == 1) {
                            newClip = "idle_right";
                        } else if (anim.lastDirection == 2) {
                            newClip = "idle_down";
                        } else if (anim.lastDirection == 3) {
                            newClip = "idle_left";
                        }
                    }
                }

                if (e->hasComponent<ProjectileTag>()) {
                   newClip = "idle_right";
                }



                // check if the animation has switched
                // if the chosen clip is different from current one, switch to new clip , reset time and frame index
                if (newClip != anim.currentClip) {
                    anim.currentClip = newClip; // switch to new clip
                    anim.time = 0.0f; // reset time 0
                    anim.currentFrame = 0; // reset frame 0
                }

                // playback system: advance the animation
                float animFrameSpeed = anim.speed; // how long each 'animation frame' should last
                auto clip = anim.clips[anim.currentClip]; // retrieve the frame from data from the current clip
                // advance time
                anim.time += dt; // every 'game loop frame' we are going to add the accumulated time

                // once enough time has passed
                if (anim.time >= animFrameSpeed) {
                    anim.time -= animFrameSpeed; // subtract animFrameSpeed (eg. 0.1f) so the extra time isn't lost

                    std::size_t totalAnimationFrames = clip.frameIndices.size();
                    // advance to the next animation frame (currentAnimationFrame + 1)
                    // wrap around % so it loops when reaching the end of the clip
                    anim.currentFrame = (anim.currentFrame + 1) % totalAnimationFrames;


                }
            }
        }
    }
};

#endif //ASSIGNMENT1_ANIMATIONSYSTEM_H