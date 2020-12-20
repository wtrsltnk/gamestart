#ifndef GAMEELAYER_H
#define GAMEELAYER_H

#include <core/assetsmanager.h>
#include <core/layer.h>
#include <scene.h>

#include <cstdint>
#include <entt/entt.hpp>

namespace gamestart
{

    class GameLayer :
        public Layer
    {
    public:
        GameLayer();

        virtual ~GameLayer();

        void SetScene(
            Scene *scene);

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

    private:
        AssetsManager _assetsManager;
        bool _isAttached = false;
        Scene *_scene = nullptr;
    };

} // namespace gamestart

#endif // GAMELAYER_H
