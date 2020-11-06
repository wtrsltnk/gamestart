#include <application.h>

#include <spdlog/spdlog.h>

using namespace gamestart;

bool Application::PlatformPreInitialize(
    int argc,
    char *argv[])
{
    return true;
}

bool Application::PlatformPostInitialize()
{
    return true;
}

void Application::PlatformPreCleanup()
{
}

void Application::PlatformPostCleanup()
{
}
