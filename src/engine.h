#pragma once

#include <chrono>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

#include "platform.h"
#include "renderer.h"

class Engine
{
    using TimeDelta = std::chrono::milliseconds;

public:
    Engine();
    ~Engine();

    bool Initialize(const std::string &appName, int width, int height, bool enableValidationLayers = true);

    void run();
    void cleanup();

private:
    std::unique_ptr<Platform> platform;
    std::unique_ptr<Renderer> renderer;

    std::thread::id mainThreadId{};

    bool initialized = false;
    bool running = false;

    std::chrono::milliseconds deltaTimeMs{0};
    uint64_t lastFrameTimeMs = 0;
};
