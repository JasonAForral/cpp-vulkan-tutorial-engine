#pragma once

#include <functional>
#include <memory>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define LOGI(...)        \
    printf(__VA_ARGS__); \
    printf("\n")
#define LOGW(...)        \
    printf(__VA_ARGS__); \
    printf("\n")
#define LOGE(...)                 \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n")

class Platform
{
public:
    Platform() = default;
    virtual ~Platform() = default;
    virtual void cleanup() = 0;
    virtual bool Initialize(const std::string &appName, int width, int height) = 0;
    virtual bool ProcessEvents() = 0;

};

class DesktopPlatform final : public Platform
{
private:
    GLFWwindow *window = nullptr;
    int width = 0;
    int height = 0;

public:
    DesktopPlatform() = default;
    void cleanup() override;
    bool Initialize(const std::string &appName, int width, int height) override;
    bool ProcessEvents() override;
};

template<typename... Args>
std::unique_ptr<Platform> CreatePlatform(Args&&... args) {
    return std::make_unique<DesktopPlatform>();
}
