//
// Created by Joseph Driedger on 3/8/2026.
//

#ifndef ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_RENDER_H
#define ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_RENDER_H

namespace game
{
    struct Render
    {
        char Symbol;

        Render()
            : Symbol('?')
        {
        }

        explicit Render(const char symbol)
            : Symbol(symbol)
        {
        }
    };
}

#endif // ECHOES_OF_THE_FORGOTTEN_KEEP_GAME_COMPONENTS_RENDER_H