#ifndef SCENELAYER_H
#define SCENELAYER_H

#include <cstdint>
#include <entt/entt.hpp>
#include <layer.h>
#include <scene.h>

namespace gamestart
{

    class SceneLayer :
        public Layer
    {
    public:
        SceneLayer();

        virtual ~SceneLayer();

        virtual void OnAttach(
            SDL_Window *window,
            SDL_GLContext context);

        virtual void OnResizeEvent(
            int width,
            int height);

        virtual bool OnEvent(
            const SDL_Event &event);

        virtual void OnUpdate(
            uint32_t time);

        virtual void OnDetach();
    };

} // namespace gamestart

#endif // SCENELAYER_H
