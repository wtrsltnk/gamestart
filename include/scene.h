#ifndef SCENE_H
#define SCENE_H

#include <cstdint>
#include <entt/entt.hpp>

namespace gamestart
{

    class Scene
    {
    public:
        Scene();

        virtual ~Scene();

        entt::entity CreateEntity(
            const std::string &title);

        void OnInitialize();

        void OnResizeEvent(
            int width,
            int height);

        void OnUpdate(
            uint32_t time);

    private:
        entt::registry m_Registry;
    };

} // namespace gamestart

#endif // SCENE_H
