//
// Created by adamd on 2026-04-03.
//

#ifndef ASSIGNMENT1_AUDIOEVENTQUEUE_H
#define ASSIGNMENT1_AUDIOEVENTQUEUE_H
#include <memory>
#include <queue>
#include <string>

#include "AudioManager.h"
namespace engine {

    // Represents a deferred sound-effect request.
    // Queued during gameplay logic and dispatched later to avoid
    // triggering audio in the middle of ECS updates.
    class AudioEvent {
    public:
        explicit AudioEvent(const std::string &name) : name(name) {}

        void execute() const {
            AudioManager::playSfx(name);
        }

    private:
        std::string name;
    };

    // FIFO queue of audio events processed once per frame.
    // Decouples game logic from direct audio calls, allowing systems to
    // enqueue sounds without holding a reference to AudioManager.
    class AudioEventQueue {
    public:

        void push(std::unique_ptr<AudioEvent> event) {
            events.push(std::move(event));
        }

        // Executes and drains all queued events in order.
        void process() {
            while (!events.empty()) {
                events.front()->execute();
                events.pop();
            }
        }

    private:
        std::queue<std::unique_ptr<AudioEvent>> events;
    };
}
#endif //ASSIGNMENT1_AUDIOEVENTQUEUE_H