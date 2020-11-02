#include <application.h>
#include <iostream>
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

    return app.Run();
}
