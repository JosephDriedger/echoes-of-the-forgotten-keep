//
// Created by adamd on 2026-02-04.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_EVENTMANAGER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_EVENTMANAGER_H
#include <functional>

#include "Entity.h"


struct CollisionEvent {
    Entity* entityA = nullptr;
    Entity* entityB = nullptr;
};

// Observer pattern
class EventManager {
public:
    // template<typename EventType>
    // void emit(const EventType& event) {
    //     // retrieve the list of subscribers to certain events
    //     auto& listeners = getListeners<EventType>();
    //     // Loop all the subscribers to certain events
    //     for (auto& listener : listeners) {
    //         listener(event); // invoke the function here
    //     }
    // }

    void emit(const CollisionEvent& event) const {
        for (const auto& listener : collisionListeners) {
            listener(event);
        }
    }

    // template<typename EventType>
    // void subscribe(std::function<void(const EventType&)> callback) {
    //     // pass in the callback/callable wrapper/subscription to the list of subscriptions
    //     getListeners<EventType>().push_back(callback);
    // }

    void subscribe(std::function<void(const CollisionEvent&)> callback) {
        collisionListeners.emplace_back(callback);
    }

private:

    // each event type essentially has its own std::vector of listeners without you having to manage it explicitly. This is done using the static local.
    // std:function<void(const EventType&)> is the callable wrapper: can hold anything that can be called like a function. (lambda, free function, functor, etc.)

    // template<typename EventType>
    // std::vector<std::function<void(const EventType)>>& getListeners() {
    //     static std::vector<std::function<void(const EventType)>> listeners;
    //     return listeners;
    // }

    std::vector<std::function<void(const CollisionEvent&)>> collisionListeners;
};

#endif //ECHOES_OF_THE_FORGOTTEN_KEEP_EVENTMANAGER_H