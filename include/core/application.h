 #ifndef APPLICATION_H
#define APPLICATION_H

#define SDL_MAIN_HANDLED
#include <SDL.h>

#include <core/layer.h>
#include <memory>
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

        bool Initialize();

        void AttachLayer(
            std::unique_ptr<Layer> layer);

        template <class TLayer>
        TLayer *GetFirstLayerOfType()
        {
            for (auto &layer : _layers)
            {
                if (typeid(*layer.get()) == typeid(TLayer))
                {
                    return (TLayer *)layer.get();
                }
            }

            return nullptr;
        }

        int Run();

#if defined(EMSCRIPTEN)
        static void MainLoopWrapper(void *arg);
#endif

    private:
        bool PlatformPreInitialize();

        bool PlatformPostInitialize();

        bool MainLoop();

        void PlatformPreCleanup();

        void PlatformPostCleanup();

        void Cleanup();

        SDL_Window *_window;

        SDL_GLContext _context;

        std::vector<std::unique_ptr<Layer>> _layers;

        const char *_title;

        int _initialWidth;

        int _initialHeight;
    };

} // namespace gamestart

#endif // APPLICATION_H
