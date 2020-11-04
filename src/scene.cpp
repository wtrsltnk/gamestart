#include <scene.h>

using namespace gamestart;

Scene::Scene() = default;

Scene::~Scene() = default;

entt::entity Scene::CreateEntity(
    const std::string &title)
{
    return entt::entity();
}

void Scene::OnAttach(
    SDL_Window *window,
    SDL_GLContext context)
{
}

void Scene::OnResizeEvent(
    int width,
    int height)
{
}

bool Scene::OnEvent(
    const SDL_Event &event)
{
    return false;
}

void Scene::OnUpdate(
    uint32_t time)
{
}

void Scene::OnDetach()
{
}
