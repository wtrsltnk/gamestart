#ifndef APPLICATION_H
#define APPLICATION_H

#include <renderer.h>
#include <scene.h>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

namespace gamestart
{

    class Application
    {
    public:
        Application(
            const char *title,
            int intialWidth,
            int initialHeight);

        virtual ~Application();

        bool Initialize(
            int argc,
            char *argv[]);

        int Run();

#if defined(EMSCRIPTEN)
    static void MainLoopWrapper(void *arg);
#endif

    private:
        bool PlatformPreInitialize(
            int argc,
            char *argv[]);

        bool PlatformPostInitialize();

        bool MainLoop();

        void PlatformPreCleanup();

        void PlatformPostCleanup();

        void Cleanup();

        SDL_Window *_window;

        SDL_GLContext _context;

        Renderer _renderer;

        Scene _scene;

        const char *_title;

        int _initialWidth;

        int _initialHeight;
    };

} // namespace gamestart

#endif // APPLICATION_H
