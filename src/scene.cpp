#include <scene.h>

#include <entities/graphicscomponent.h>
#include <entities/namecomponent.h>
#include <entities/transformcomponent.h>
#include <entt/entt.hpp>
#include <glad/glad.h>

using namespace gamestart;

struct LoadedGraphicsAssetComponent
{
    std::shared_ptr<LoadedAsset> asset;
};

Scene::Scene() = default;

Scene::~Scene() = default;

entt::entity Scene::CreateEntity(
    const std::string &title)
{
    auto result = m_Registry.create();

    m_Registry.emplace<NameComponent>(result, title);

    m_Registry.emplace<TransformComponent>(result);

    return result;
}

void Scene::Initialize(
    AssetsManager &assetsManager)
{
    auto view = m_Registry.view<GraphicsComponent>();

    for (auto entity : view)
    {
        auto graphicsComponent = m_Registry.get<GraphicsComponent>(entity);

        auto loadedAsset = assetsManager.LoadAsset(graphicsComponent.asset);

        m_Registry.emplace<LoadedGraphicsAssetComponent>(entity, loadedAsset);
    }
}

void Scene::OnResizeEvent(
    int width,
    int height)
{
    (void)width;
    (void)height;
}

void Scene::OnUpdate(
    uint32_t time)
{
    (void)time;
}

void Scene::Cleanup(
    AssetsManager &assetsManager)
{
    auto view = m_Registry.view<LoadedGraphicsAssetComponent>();

    for (auto entity : view)
    {
        auto graphicsComponent = m_Registry.get<LoadedGraphicsAssetComponent>(entity);

        assetsManager.UnloadAsset(graphicsComponent.asset);

        m_Registry.remove<LoadedGraphicsAssetComponent>(entity);
    }
}
