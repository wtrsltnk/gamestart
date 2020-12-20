#ifndef LAYER_H
#define LAYER_H

#include <SDL.h>
#include <cstdint>

namespace gamestart
{

    class Layer
    {
    public:
        virtual ~Layer();

        virtual void OnAttach(
            SDL_Window *window,
            SDL_GLContext context) = 0;

        virtual void OnResizeEvent(
            int width,
            int height) = 0;

        virtual bool OnEvent(
            const SDL_Event &event) = 0;

        virtual void OnUpdate(
            uint32_t time) = 0;

        virtual void OnDetach() = 0;
    };

} // namespace gamestart

#endif // LAYER_H
