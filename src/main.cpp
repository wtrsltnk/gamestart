#include <application.h>
#include <imguilayer.h>
#include <iostream>
#include <memory>
#include <scenelayer.h>
#include <string>
#include <vector>

int main(
    int argc,
    char *argv[])
{
    gamestart::Application app(
        "Game Start",
        1024, 768);

    if (!app.Initialize(argc, argv))
    {
        return 0;
    }

    app.AttachLayer(
        std::unique_ptr<gamestart::SceneLayer>(new gamestart::SceneLayer()));

    app.AttachLayer(
        std::unique_ptr<gamestart::ImGuiLayer>(new gamestart::ImGuiLayer([]() {
            ImGui::ShowDemoWindow();
        })));

    return app.Run();
}
