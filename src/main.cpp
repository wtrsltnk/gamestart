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
        std::cerr << "failed to initialize app" << std::endl;
        return 1;
    }

    app.AttachLayer(
        new gamestart::SceneLayer());

    app.AttachLayer(
        new gamestart::ImGuiLayer([]() {
            ImGui::ShowDemoWindow();
        }));

    return app.Run();
}
