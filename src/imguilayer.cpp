#include <imguilayer.h>

#include <glad/glad.h>

#include "imgui_impl_opengl3.h"

#include <SDL_syswm.h>

#define SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE SDL_VERSION_ATLEAST(2, 0, 4)

using namespace gamestart;

ImGuiLayer::ImGuiLayer(
    std::function<void()> drawUi)
    : _drawUi(drawUi)
{}

ImGuiLayer::~ImGuiLayer() = default;

const char *ImGuiLayer::ImGui_ImplSDL2_GetClipboardText(
    void *userData)
{
    ImGuiLayer *layer = reinterpret_cast<ImGuiLayer *>(userData);

    if (layer->_clipboardTextData)
    {
        SDL_free(layer->_clipboardTextData);
    }

    layer->_clipboardTextData = SDL_GetClipboardText();

    return layer->_clipboardTextData;
}

void ImGuiLayer::ImGui_ImplSDL2_SetClipboardText(
    void *,
    const char *text)
{
    SDL_SetClipboardText(text);
}

void ImGuiLayer::OnAttach(
    SDL_Window *window,
    SDL_GLContext context)
{
    _window = window;
    _context = context;

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup back-end capabilities flags
    ImGuiIO &io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors; // We can honor GetMouseCursor() values (optional)
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;  // We can honor io.WantSetMousePos requests (optional, rarely used)
    io.BackendPlatformName = "imgui_impl_sdl";

    ImGui::StyleColorsDark();

    // Keyboard mapping. ImGui will use those indices to peek into the io.KeysDown[] array.
    io.KeyMap[ImGuiKey_Tab] = SDL_SCANCODE_TAB;
    io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
    io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
    io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
    io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
    io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
    io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
    io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
    io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
    io.KeyMap[ImGuiKey_Insert] = SDL_SCANCODE_INSERT;
    io.KeyMap[ImGuiKey_Delete] = SDL_SCANCODE_DELETE;
    io.KeyMap[ImGuiKey_Backspace] = SDL_SCANCODE_BACKSPACE;
    io.KeyMap[ImGuiKey_Space] = SDL_SCANCODE_SPACE;
    io.KeyMap[ImGuiKey_Enter] = SDL_SCANCODE_RETURN;
    io.KeyMap[ImGuiKey_Escape] = SDL_SCANCODE_ESCAPE;
    io.KeyMap[ImGuiKey_KeyPadEnter] = SDL_SCANCODE_KP_ENTER;
    io.KeyMap[ImGuiKey_A] = SDL_SCANCODE_A;
    io.KeyMap[ImGuiKey_C] = SDL_SCANCODE_C;
    io.KeyMap[ImGuiKey_V] = SDL_SCANCODE_V;
    io.KeyMap[ImGuiKey_X] = SDL_SCANCODE_X;
    io.KeyMap[ImGuiKey_Y] = SDL_SCANCODE_Y;
    io.KeyMap[ImGuiKey_Z] = SDL_SCANCODE_Z;

    io.SetClipboardTextFn = ImGuiLayer::ImGui_ImplSDL2_SetClipboardText;
    io.GetClipboardTextFn = ImGuiLayer::ImGui_ImplSDL2_GetClipboardText;
    io.ClipboardUserData = NULL;

    // Load mouse cursors
    _mouseCursors[ImGuiMouseCursor_Arrow] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    _mouseCursors[ImGuiMouseCursor_TextInput] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    _mouseCursors[ImGuiMouseCursor_ResizeAll] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
    _mouseCursors[ImGuiMouseCursor_ResizeNS] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
    _mouseCursors[ImGuiMouseCursor_ResizeEW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
    _mouseCursors[ImGuiMouseCursor_ResizeNESW] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
    _mouseCursors[ImGuiMouseCursor_ResizeNWSE] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
    _mouseCursors[ImGuiMouseCursor_Hand] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    _mouseCursors[ImGuiMouseCursor_NotAllowed] = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);

    // Check and store if we are on Wayland
    _mouseCanUseGlobalState = strncmp(SDL_GetCurrentVideoDriver(), "wayland", 7) != 0;

#ifdef _WIN32
    SDL_SysWMinfo wmInfo;
    SDL_VERSION(&wmInfo.version);
    SDL_GetWindowWMInfo(window, &wmInfo);
    io.ImeWindowHandle = wmInfo.info.win.window;
#else
    (void)window;
#endif

    ImGui_ImplOpenGL3_Init("#version 150");
}

void ImGuiLayer::OnResizeEvent(
    int width,
    int height)
{
}

bool ImGuiLayer::OnEvent(
    const SDL_Event &event)
{
    ImGuiIO &io = ImGui::GetIO();
    switch (event.type)
    {
        case SDL_MOUSEWHEEL:
        {
            if (event.wheel.x > 0) io.MouseWheelH += 1;
            if (event.wheel.x < 0) io.MouseWheelH -= 1;
            if (event.wheel.y > 0) io.MouseWheel += 1;
            if (event.wheel.y < 0) io.MouseWheel -= 1;
            return true;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            if (event.button.button == SDL_BUTTON_LEFT) _mousePressed[0] = true;
            if (event.button.button == SDL_BUTTON_RIGHT) _mousePressed[1] = true;
            if (event.button.button == SDL_BUTTON_MIDDLE) _mousePressed[2] = true;
            return true;
        }
        case SDL_TEXTINPUT:
        {
            io.AddInputCharactersUTF8(event.text.text);
            return true;
        }
        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            int key = event.key.keysym.scancode;
            IM_ASSERT(key >= 0 && key < IM_ARRAYSIZE(io.KeysDown));
            io.KeysDown[key] = (event.type == SDL_KEYDOWN);
            io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
            io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
            io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
#ifdef _WIN32
            io.KeySuper = false;
#else
            io.KeySuper = ((SDL_GetModState() & KMOD_GUI) != 0);
#endif
            return true;
        }
    }
    return false;
}

void ImGuiLayer::OnUpdate(
    uint32_t time)
{
    ImGui_ImplOpenGL3_NewFrame();

    ImGuiIO &io = ImGui::GetIO();
    IM_ASSERT(io.Fonts->IsBuilt() && "Font atlas not built! It is generally built by the renderer back-end. Missing call to renderer _NewFrame() function? e.g. ImGui_ImplOpenGL3_NewFrame().");

    // Setup display size (every frame to accommodate for window resizing)
    int w, h;
    int display_w, display_h;
    SDL_GetWindowSize(_window, &w, &h);
    if (SDL_GetWindowFlags(_window) & SDL_WINDOW_MINIMIZED)
    {
        w = h = 0;
    }

    SDL_GL_GetDrawableSize(_window, &display_w, &display_h);
    io.DisplaySize = ImVec2((float)w, (float)h);
    if (w > 0 && h > 0)
    {
        io.DisplayFramebufferScale = ImVec2((float)display_w / w, (float)display_h / h);
    }

    // Setup time step (we don't use SDL_GetTicks() because it is using millisecond resolution)
    static Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 current_time = SDL_GetPerformanceCounter();
    io.DeltaTime = _time > 0 ? (float)((double)(current_time - _time) / frequency) : (float)(1.0f / 60.0f);
    _time = current_time;

    ImGui_ImplSDL2_UpdateMousePosAndButtons(io);
    ImGui_ImplSDL2_UpdateMouseCursor(io);

    // Update game controllers (if enabled and available)
    ImGui_ImplSDL2_UpdateGamepads(io);

    ImGui::NewFrame();

    _drawUi();

    ImGui::Render();

    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::ImGui_ImplSDL2_UpdateMousePosAndButtons(
    ImGuiIO &io)
{
    // Set OS mouse position if requested (rarely used, only when ImGuiConfigFlags_NavEnableSetMousePos is enabled by user)
    if (io.WantSetMousePos)
    {
        SDL_WarpMouseInWindow(_window, (int)io.MousePos.x, (int)io.MousePos.y);
    }
    else
    {
        io.MousePos = ImVec2(-FLT_MAX, -FLT_MAX);
    }

    int mx, my;
    Uint32 mouse_buttons = SDL_GetMouseState(&mx, &my);
    io.MouseDown[0] = _mousePressed[0] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_LEFT)) != 0; // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
    io.MouseDown[1] = _mousePressed[1] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_RIGHT)) != 0;
    io.MouseDown[2] = _mousePressed[2] || (mouse_buttons & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;
    _mousePressed[0] = _mousePressed[1] = _mousePressed[2] = false;

#if SDL_HAS_CAPTURE_AND_GLOBAL_MOUSE && !defined(__EMSCRIPTEN__) && !defined(__ANDROID__) && !(defined(__APPLE__) && TARGET_OS_IOS)
    SDL_Window *focused_window = SDL_GetKeyboardFocus();
    if (_window == focused_window)
    {
        if (_mouseCanUseGlobalState)
        {
            // SDL_GetMouseState() gives mouse position seemingly based on the last window entered/focused(?)
            // The creation of a new windows at runtime and SDL_CaptureMouse both seems to severely mess up with that, so we retrieve that position globally.
            // Won't use this workaround when on Wayland, as there is no global mouse position.
            int wx, wy;
            SDL_GetWindowPosition(focused_window, &wx, &wy);
            SDL_GetGlobalMouseState(&mx, &my);
            mx -= wx;
            my -= wy;
        }
        io.MousePos = ImVec2((float)mx, (float)my);
    }

    // SDL_CaptureMouse() let the OS know e.g. that our imgui drag outside the SDL window boundaries shouldn't e.g. trigger the OS window resize cursor.
    // The function is only supported from SDL 2.0.4 (released Jan 2016)
    bool any_mouse_button_down = ImGui::IsAnyMouseDown();
    SDL_CaptureMouse(any_mouse_button_down ? SDL_TRUE : SDL_FALSE);
#else
    if (SDL_GetWindowFlags(_window) & SDL_WINDOW_INPUT_FOCUS)
    {
        io.MousePos = ImVec2((float)mx, (float)my);
    }
#endif
}

void ImGuiLayer::ImGui_ImplSDL2_UpdateMouseCursor(
    ImGuiIO &io)
{
    if (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange)
        return;

    ImGuiMouseCursor imgui_cursor = ImGui::GetMouseCursor();
    if (io.MouseDrawCursor || imgui_cursor == ImGuiMouseCursor_None)
    {
        // Hide OS mouse cursor if imgui is drawing it or if it wants no cursor
        SDL_ShowCursor(SDL_FALSE);
    }
    else
    {
        // Show OS mouse cursor
        SDL_SetCursor(_mouseCursors[imgui_cursor] ? _mouseCursors[imgui_cursor] : _mouseCursors[ImGuiMouseCursor_Arrow]);
        SDL_ShowCursor(SDL_TRUE);
    }
}

void ImGuiLayer::ImGui_ImplSDL2_UpdateGamepads(
    ImGuiIO &io)
{
    memset(io.NavInputs, 0, sizeof(io.NavInputs));
    if ((io.ConfigFlags & ImGuiConfigFlags_NavEnableGamepad) == 0)
        return;

    // Get gamepad
    SDL_GameController *game_controller = SDL_GameControllerOpen(0);
    if (!game_controller)
    {
        io.BackendFlags &= ~ImGuiBackendFlags_HasGamepad;
        return;
    }

// Update gamepad inputs
#define MAP_BUTTON(NAV_NO, BUTTON_NO)                                                                        \
    {                                                                                                        \
        io.NavInputs[NAV_NO] = (SDL_GameControllerGetButton(game_controller, BUTTON_NO) != 0) ? 1.0f : 0.0f; \
    }
#define MAP_ANALOG(NAV_NO, AXIS_NO, V0, V1)                                                              \
    {                                                                                                    \
        float vn = (float)(SDL_GameControllerGetAxis(game_controller, AXIS_NO) - V0) / (float)(V1 - V0); \
        if (vn > 1.0f) vn = 1.0f;                                                                        \
        if (vn > 0.0f && io.NavInputs[NAV_NO] < vn) io.NavInputs[NAV_NO] = vn;                           \
    }
    const int thumb_dead_zone = 8000;                                         // SDL_gamecontroller.h suggests using this value.
    MAP_BUTTON(ImGuiNavInput_Activate, SDL_CONTROLLER_BUTTON_A);              // Cross / A
    MAP_BUTTON(ImGuiNavInput_Cancel, SDL_CONTROLLER_BUTTON_B);                // Circle / B
    MAP_BUTTON(ImGuiNavInput_Menu, SDL_CONTROLLER_BUTTON_X);                  // Square / X
    MAP_BUTTON(ImGuiNavInput_Input, SDL_CONTROLLER_BUTTON_Y);                 // Triangle / Y
    MAP_BUTTON(ImGuiNavInput_DpadLeft, SDL_CONTROLLER_BUTTON_DPAD_LEFT);      // D-Pad Left
    MAP_BUTTON(ImGuiNavInput_DpadRight, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);    // D-Pad Right
    MAP_BUTTON(ImGuiNavInput_DpadUp, SDL_CONTROLLER_BUTTON_DPAD_UP);          // D-Pad Up
    MAP_BUTTON(ImGuiNavInput_DpadDown, SDL_CONTROLLER_BUTTON_DPAD_DOWN);      // D-Pad Down
    MAP_BUTTON(ImGuiNavInput_FocusPrev, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);  // L1 / LB
    MAP_BUTTON(ImGuiNavInput_FocusNext, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER); // R1 / RB
    MAP_BUTTON(ImGuiNavInput_TweakSlow, SDL_CONTROLLER_BUTTON_LEFTSHOULDER);  // L1 / LB
    MAP_BUTTON(ImGuiNavInput_TweakFast, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER); // R1 / RB
    MAP_ANALOG(ImGuiNavInput_LStickLeft, SDL_CONTROLLER_AXIS_LEFTX, -thumb_dead_zone, -32768);
    MAP_ANALOG(ImGuiNavInput_LStickRight, SDL_CONTROLLER_AXIS_LEFTX, +thumb_dead_zone, +32767);
    MAP_ANALOG(ImGuiNavInput_LStickUp, SDL_CONTROLLER_AXIS_LEFTY, -thumb_dead_zone, -32767);
    MAP_ANALOG(ImGuiNavInput_LStickDown, SDL_CONTROLLER_AXIS_LEFTY, +thumb_dead_zone, +32767);

    io.BackendFlags |= ImGuiBackendFlags_HasGamepad;
#undef MAP_BUTTON
#undef MAP_ANALOG
}

void ImGuiLayer::OnDetach()
{
    // Destroy last known clipboard data
    if (_clipboardTextData != nullptr)
    {
        SDL_free(_clipboardTextData);
    }

    _clipboardTextData = nullptr;

    // Destroy SDL mouse cursors
    for (ImGuiMouseCursor cursor_n = 0; cursor_n < ImGuiMouseCursor_COUNT; cursor_n++)
    {
        SDL_FreeCursor(_mouseCursors[cursor_n]);
    }

    memset(_mouseCursors, 0, sizeof(_mouseCursors));

    ImGui::DestroyContext();
}
