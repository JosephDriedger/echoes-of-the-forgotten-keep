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

    class AudioEvent {
    public:
        explicit AudioEvent(const std::string &name) : name(name) {}

        void execute() const {
            AudioManager::playSfx(name);
        }

    private:
        std::string name;
    };

    class AudioEventQueue {
    public:

        void push(std::unique_ptr<AudioEvent> event) {
            events.push(std::move(event));
        }

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