#pragma once

#include <functional>
#include <memory>
#include <string>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Platform
{
public:
    Platform() = default;
    virtual ~Platform() = default;
    virtual void cleanup() = 0;
};

class DesktopPlatform final : public Platform
{
private:
    GLFWwindow* window = nullptr;
    int width = 0;
    int height = 0;

public:
    DesktopPlatform() = default;
    void cleanup() override;
};
