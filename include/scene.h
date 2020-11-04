#ifndef SCENE_H
#define SCENE_H

#include <cstdint>
#include <entt/entt.hpp>
#include <layer.h>

namespace gamestart
{

    class Scene :
        public Layer
    {
    public:
        Scene();

        virtual ~Scene();

        entt::entity CreateEntity(
            const std::string &title);

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
        entt::registry m_Registry;
    };

} // namespace gamestart

#endif // SCENE_H
