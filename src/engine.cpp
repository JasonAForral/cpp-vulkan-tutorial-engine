#include "engine.h"

Engine::Engine()
{
}

Engine::~Engine()
{
    cleanup();
}

void Engine::run()
{
}

void Engine::cleanup()
{
    if (initialized) {
        if (renderer) {
            renderer->cleanup();
        }
    }
}
