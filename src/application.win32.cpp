#include <core/application.h>

#include <spdlog/spdlog.h>

using namespace gamestart;

bool Application::PlatformPreInitialize()
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
