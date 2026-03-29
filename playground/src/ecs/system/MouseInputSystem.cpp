//
// Created by scept on 2026-03-14.
//

#include "MouseInputSystem.h"
#include "World.h"

void MouseInputSystem::update(World &world, const SDL_Event &event) {
    if (event.type != SDL_EVENT_MOUSE_MOTION &&
        event.type != SDL_EVENT_MOUSE_BUTTON_DOWN &&
        event.type != SDL_EVENT_MOUSE_BUTTON_UP) {
        return;
    }

    float mx, my;
    SDL_GetMouseState(&mx, &my);

    for (auto& e : world.getEntities()) {
        if (e->hasComponent<Clickable>() && e->hasComponent<Collider>()) {
            Clickable& clickable = e->getComponent<Clickable>();
            Collider& collider = e->getComponent<Collider>();

            if (!collider.enabled) {continue;}

            bool inside = (mx >= collider.rect.x && mx <= collider.rect.x + collider.rect.w &&
                my >= collider.rect.y && my <= collider.rect.y + collider.rect.h);

            // Hover
            if (event.type == SDL_EVENT_MOUSE_MOTION) {
                if (!inside && clickable.pressed) {
                    // cancel event
                    world.getEventManager().emit(MouseInteractionEvent{e.get(), MouseInteractionState::Cancel});
                }
            }

            // Pressed
            if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (inside) {
                        clickable.pressed = true;
                        // pressed event
                        world.getEventManager().emit(MouseInteractionEvent{e.get(), MouseInteractionState::Pressed});
                    }
                }
            }

            // Released
            if (event.type == SDL_EVENT_MOUSE_BUTTON_UP) {
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (inside) {
                        clickable.pressed = false;
                        // released event
                        world.getEventManager().emit(MouseInteractionEvent{e.get(), MouseInteractionState::Released});
                    }
                }
            }
        }
    }
}
