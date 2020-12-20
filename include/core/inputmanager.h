#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <cstdint>
#include <functional>

namespace gamestart
{

    class InputManager
    {
    public:
        InputManager();

        void OnRawKeyboardInput(
            uint8_t keyCode,
            bool downAction);

        void OnRawMouseInput(
            uint8_t buttonCode,
            bool downAction);

        void OnRawMouseMotion(
            int x,
            int y);
    };

} // namespace gamestart

#endif // INPUTMANAGER_H
