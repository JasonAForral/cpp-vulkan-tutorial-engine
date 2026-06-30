#include "engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
    cleanup();
}

bool Engine::Initialize(const std::string &appName, int width, int height, bool enableValidationLayers)
{
    mainThreadId = std::this_thread::get_id();

    platform = CreatePlatform();
    if (!platform->Initialize(appName, width, height))
    {
        std::cout << "Failed to initialize platform" << std::endl;
        return false;
    }

    renderer = std::make_unique<Renderer>(platform.get());

    if (!renderer->Initialize(appName, enableValidationLayers))
    {
        std::cout << "Failed to initialize renderer" << std::endl;
        return false;
    }

    initialized = true;
    std::cout << "Platform initialized successfully" << std::endl;
    return true;
}

void Engine::run()
{
    if (!initialized)
    {
        throw std::runtime_error("Engine not initialized");
    }

    running = true;

    std::cout << "Engine run loop started" << std::endl;

    while (running)
    {
        if (!platform->ProcessEvents())
        {
            running = false;
            break;
        }
    }

    std::cout << "Engine run loop exited" << std::endl;
}

void Engine::cleanup()
{
    if (initialized)
    {
        if (renderer)
        {
            renderer->cleanup();
        }
    }
}
