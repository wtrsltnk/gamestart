#ifndef IMGUILAYER_H
#define IMGUILAYER_H

#include <functional>
#include <imgui.h>
#include <layer.h>

namespace gamestart
{

    class ImGuiLayer :
        public Layer
    {
    public:
        ImGuiLayer(
            std::function<void()> drawUi);

        virtual ~ImGuiLayer();

        virtual void OnAttach(
            SDL_Window *window,
            SDL_GLContext context);

        virtual void OnResizeEvent(
            int width,
            int height);

        virtual bool OnEvent(
            const SDL_Event &event);

        virtual void OnUpdate(
            uint32_t time);

        virtual void OnDetach();

    private:
        SDL_Window *_window = nullptr;
        SDL_GLContext _context = nullptr;
        Uint64 _time = 0;
        bool _mousePressed[3] = {false, false, false};
        SDL_Cursor *_mouseCursors[ImGuiMouseCursor_COUNT] = {};
        char *_clipboardTextData = nullptr;
        bool _mouseCanUseGlobalState = true;
        std::function<void()> _drawUi;

        static const char *ImGui_ImplSDL2_GetClipboardText(
            void *userData);

        static void ImGui_ImplSDL2_SetClipboardText(
            void *userData,
            const char *text);

        void ImGui_ImplSDL2_UpdateMousePosAndButtons(
            ImGuiIO &io);

        void ImGui_ImplSDL2_UpdateMouseCursor(
            ImGuiIO &io);

        void ImGui_ImplSDL2_UpdateGamepads(
            ImGuiIO &io);
    };

} // namespace gamestart

#endif // IMGUILAYER_H
