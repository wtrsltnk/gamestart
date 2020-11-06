#include <scene.h>

using namespace gamestart;

Scene::Scene() = default;

Scene::~Scene() = default;

entt::entity Scene::CreateEntity(
    const std::string &title)
{
    return entt::entity();
}

void Scene::Initialize()
{
}

void Scene::OnResizeEvent(
    int width,
    int height)
{
}

void Scene::OnUpdate(
    uint32_t time)
{
}

void Scene::Cleanup()
{
}
