#ifndef SCENE_H
#define SCENE_H

#include <core/assetsmanager.h>

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

        virtual void Initialize(
            AssetsManager &assetsManager);

        virtual void OnResizeEvent(
            int width,
            int height);

        virtual void OnUpdate(
            uint32_t time);

        virtual void Cleanup(
            AssetsManager &assetsManager);

    private:
        entt::registry m_Registry;
    };

} // namespace gamestart

#endif // SCENE_H
