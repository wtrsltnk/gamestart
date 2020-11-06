#include <scenelayer.h>

using namespace gamestart;

SceneLayer::SceneLayer() = default;

SceneLayer::~SceneLayer() = default;

void SceneLayer::OnAttach(
    SDL_Window *window,
    SDL_GLContext context)
{
}

void SceneLayer::OnResizeEvent(
    int width,
    int height)
{
}

bool SceneLayer::OnEvent(
    const SDL_Event &event)
{
    return false;
}

void SceneLayer::OnUpdate(
    uint32_t time)
{
}

void SceneLayer::OnDetach()
{
}
