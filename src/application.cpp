#include <application.h>

#include <glad/glad.h>
#include <imgui_impl_sdl.h>
#include <iostream>
#include <lyra/lyra.hpp>
#include <spdlog/spdlog.h>

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
{
    spdlog::set_level(spdlog::level::debug);
}

Application::~Application() = default;

bool Application::Initialize(
    int argc,
    char *argv[])
{
    int width = _initialWidth, height = _initialHeight;
    std::string logLevel;
    uint32_t flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;

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
        
        return false;
    }

    spdlog::debug("Initialize");

    if (!result)
    {
        spdlog::error("arguments parsing failed with message: {0}", result.errorMessage());

        return false;
    }

    if (!PlatformPreInitialize(argc, argv))
    {
        spdlog::error("platform pre initialize failed");

        return false;
    }

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        spdlog::error("initializing SDL failed");

        return false;
    }

    // For antialiasing
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    // the minimum number of bits in the depth buffer; defaults to 16
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

#if defined(__ANDROID__)
    flags |= SDL_WINDOW_FULLSCREEN;
#endif

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

    SDL_GL_MakeCurrent(_window, _context);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    if (!gladLoadGL())
    {
        spdlog::error("glad load gl failed");

        Cleanup();

        return false;
    }

    if (!PlatformPostInitialize())
    {
        spdlog::error("platform post initialize failed");

        Cleanup();

        return false;
    }

    return true;
}

void Application::AttachLayer(
    std::unique_ptr<Layer> layer)
{
    spdlog::debug("Attach layer");

    SDL_GL_MakeCurrent(_window, _context);

    layer->OnAttach(_window, _context);

    _layers.push_back(std::move(layer));
}

#if defined(EMSCRIPTEN)
void Application::MainLoopWrapper(void *arg)
{
    static_cast<Application *>(arg)->MainLoop();
}
#endif

int Application::Run()
{
    spdlog::debug("Run");

#if defined(EMSCRIPTEN)
    emscripten_set_main_loop_arg(Application::MainLoopWrapper, this, 0, 0);
    return 0;
#endif

    spdlog::debug("Starting mainloop");

    while (true)
    {
        if (!MainLoop())
        {
            break;
        }
    }

    spdlog::debug("Mainloop finished");

    Cleanup();

    spdlog::debug("Finished Run");

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
                spdlog::debug("Window close event recieved, stopping the mainloop");

                running = false;
            }
            else if (event.window.event == SDL_WINDOWEVENT_RESIZED)
            {
                SDL_GL_MakeCurrent(_window, _context);

                for (auto &layer : _layers)
                {
                    layer->OnResizeEvent(
                        event.window.data1,
                        event.window.data2);
                }
            }
            else
            {
                for (auto itr = _layers.rbegin(); itr != _layers.rend(); ++itr)
                {
                    if ((*itr)->OnEvent(event))
                    {
                        break;
                    }
                }
            }
        }
    }

    SDL_GL_MakeCurrent(_window, _context);

    glClearColor(0.49f, 0.62f, 0.75f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (auto &layer : _layers)
    {
        layer->OnUpdate(SDL_GetTicks());
    }

    SDL_GL_SwapWindow(_window);

    return running;
}

void Application::Cleanup()
{
    spdlog::debug("Cleanup");

    PlatformPreCleanup();

    for (auto itr = _layers.rbegin(); itr != _layers.rend(); ++itr)
    {
        (*itr)->OnDetach();
    }
    
    _layers.clear();
    
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
