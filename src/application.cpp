#include <application.h>

#include <lyra/lyra.hpp>
#include <glad/glad.h>
#include <iostream>

#if defined(EMSCRIPTEN)
#include <emscripten.h>
#endif

using namespace gamestart;

Application::Application(
    const char *title,
    int initialWidth,
    int initialHeight)
    : _title(title),
      _initialWidth(initialWidth),
      _initialHeight(initialHeight)
{}

Application::~Application() = default;

bool Application::Initialize(
    int argc,
    char *argv[])
{
    if (!PlatformPreInitialize(argc, argv))
    {
        spdlog::error("pre initialize failed");

        return false;
    }

    int width = _initialWidth, height = _initialHeight;
    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

#if defined(__ANDROID__)
    flags |= SDL_WINDOW_FULLSCREEN;
#endif

    bool show_help = false;
    auto cli = lyra::help(show_help) |
               lyra::opt(width, "width")
                   ["-w"]["--width"]("Game window width") |
               lyra::opt(height, "height")
                   ["-h"]["--height"]("Game window height");

    auto result = cli.parse(lyra::args(argc, argv));

    if (show_help)
    {
        std::cout << cli << std::endl;
    }

    if (!result)
    {
        spdlog::error("arguments parsing failed with message: {0}", result.errorMessage());

        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0)
    {
        spdlog::error("initializing SDL failed");

        return false;
    }

    // For antialiasing
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    // the minimum number of bits in the depth buffer; defaults to 16
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    _window = SDL_CreateWindow(
        _title,
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        flags);

    if (_window == nullptr)
    {
        spdlog::error("creating window failed");

        Cleanup();

        return false;
    }

    _context = SDL_GL_CreateContext(_window);
    if (_context == nullptr)
    {
        spdlog::error("creating GL context failed");

        Cleanup();

        return false;
    }

    if (!gladLoadGL())
    {
        spdlog::error("glad load gl failed");

        Cleanup();

        return false;
    }
    
    _scene.OnInitialize();

    if (!PlatformPostInitialize())
    {
        spdlog::error("post initialize failed");

        Cleanup();

        return false;
    }

    return true;
}

#if defined(EMSCRIPTEN)
void Application::MainLoopWrapper(void *arg)
{
    static_cast<Application*>(arg)->MainLoop();
}
#endif

int Application::Run()
{
#if defined(EMSCRIPTEN)
    emscripten_set_main_loop_arg(Application::MainLoopWrapper, this, 0, 0);
    return 0;
#endif

    while (true)
    {
        if (!MainLoop())
        {
            break;
        }
    }

    Cleanup();

    return 0;
}

bool Application::MainLoop()
{
    SDL_Event event;
    bool running = true;

    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_WINDOWEVENT)
        {
            if (event.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                running = false;
            }
            else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                SDL_GL_MakeCurrent(_window, _context);

                _scene.OnResizeEvent(
                    event.window.data1,
                    event.window.data2);
            }
        }
    }

    SDL_GL_MakeCurrent(_window, _context);

    _scene.OnUpdate(SDL_GetTicks());

    SDL_GL_SwapWindow(_window);

    return running;
}

void Application::Cleanup()
{
    PlatformPreCleanup();

    if (_context != nullptr)
    {
        SDL_GL_DeleteContext(_context);

        _context = nullptr;
    }

    if (_window != nullptr)
    {
        SDL_DestroyWindow(_window);

        _window = nullptr;
    }

    SDL_Quit();

    PlatformPostCleanup();
}
