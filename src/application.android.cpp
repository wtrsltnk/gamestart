#include <application.h>
#include <spdlog/sinks/android_sink.h>

using namespace gamestart;

bool Application::PlatformPreInitialize(
    const std::vector<std::string> &args)
{
    spdlog::android_logger_mt("android", "gamestart");

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
