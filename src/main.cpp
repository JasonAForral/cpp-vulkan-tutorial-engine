
#include <iostream>
#include <stdexcept>
#include <thread>

#include "engine.h"

constexpr int WINDOW_WIDTH = 1920;
constexpr int WINDOW_HEIGHT = 1080;
#if defined(NDEBUG)
constexpr bool ENABLE_VALIDATION_LAYERS = false;
#else
constexpr bool ENABLE_VALIDATION_LAYERS = true;
#endif

int main(int argc, char *argv[])
{
    try
    {
        std::cout << "Hello Vulkan!" << std::endl;
        Engine engine;

        if (!engine.Initialize("Simple Engine", WINDOW_WIDTH, WINDOW_HEIGHT, ENABLE_VALIDATION_LAYERS))
        {
            throw std::runtime_error("Failed to initialize engine");
        }

        engine.run();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}
