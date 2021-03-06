#include <core/application.h>
#include <core/gamelayer.h>
#include <core/imguilayer.h>
#include <iostream>
#include <lyra/lyra.hpp>
#include <memory>
#include <spdlog/spdlog.h>
#include <string>
#include <vector>

int main(
    int argc,
    char *argv[])
{
    int width = 1024, height = 768;
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

        return 1;
    }

    if (!result)
    {
        spdlog::error("arguments parsing failed with message: {0}", result.errorMessage());

        std::cout << cli << std::endl;

        return 0;
    }

    gamestart::Application app(
        "Game Start",
        width,
        height);

    if (!app.Initialize())
    {
        return 0;
    }

    app.AttachLayer(
        std::unique_ptr<gamestart::GameLayer>(new gamestart::GameLayer()));

    app.AttachLayer(
        std::unique_ptr<gamestart::ImGuiLayer>(new gamestart::ImGuiLayer([](uint32_t fps) {
            ImGui::ShowDemoWindow();
        })));

    auto gameLayer = app.GetFirstLayerOfType<gamestart::GameLayer>();

    gamestart::Scene scene;

    auto test = scene.CreateEntity("tree");
    scene.SetEntityAsset(test, "tree.obj");

    gameLayer->SetScene(&scene);

    return app.Run();
}
