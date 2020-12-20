#include <core/gamelayer.h>

using namespace gamestart;

GameLayer::GameLayer() = default;

GameLayer::~GameLayer() = default;

void GameLayer::SetScene(
    Scene *scene)
{
    if (_scene != nullptr)
    {
        if (_isAttached)
        {
            _scene->Cleanup(_assetsManager);
        }

        _scene = nullptr;
    }

    _scene = scene;

    if (_isAttached && _scene != nullptr)
    {
        _scene->Initialize(_assetsManager);
    }
}

void GameLayer::OnAttach(
    SDL_Window *window,
    SDL_GLContext context)
{
    if (_isAttached)
    {
        return;
    }

    if (_scene != nullptr)
    {
        _scene->Initialize(_assetsManager);
    }

    _isAttached = true;
}

void GameLayer::OnResizeEvent(
    int width,
    int height)
{
    if (_scene != nullptr)
    {
        _scene->OnResizeEvent(width, height);
    }
}

bool GameLayer::OnEvent(
    const SDL_Event &event)
{
    return false;
}

void GameLayer::OnUpdate(
    uint32_t time)
{
    if (_scene != nullptr)
    {
        _scene->OnUpdate(time);
    }
}

void GameLayer::OnDetach()
{
    if (_scene != nullptr)
    {
        _scene->Cleanup(_assetsManager);
    }

    _isAttached = false;
}
