#include "platform.h"

#include <iostream>

void DesktopPlatform::cleanup()
{
    if (window) {
        glfwDestroyWindow(window);
        window = nullptr;
    }
    glfwTerminate();
}

bool DesktopPlatform::Initialize(const std::string &appName, int width, int height)
{
    if (!glfwInit())
    {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(width, height, appName.c_str(), nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwGetFramebufferSize(window, &width, &height);

    std::cout << "GLFW window created: " << width << "x" << height << std::endl;

    return true;
}

bool DesktopPlatform::ProcessEvents()
{
    glfwPollEvents();
    return !glfwWindowShouldClose(window);
}
