#include "renderer.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

Renderer::Renderer(Platform *platform)
{
    deviceExtensions = requiredDeviceExtensions;
}

Renderer::~Renderer()
{
    cleanup();
}

bool Renderer::Initialize(const std::string &appName, bool enableValidationLayers)
{
    VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);
    if (!createInstance(appName, enableValidationLayers))
    {
        std::cout << "Failed to create Vulkan instance" << std::endl;
        return false;
    }
    return true;
}

void Renderer::cleanup()
{
}

bool Renderer::createInstance(const std::string &appName, bool enableValidationLayers)
{
    try
    {

        vk::ApplicationInfo appInfo{
            .pApplicationName = appName.c_str(),
            .applicationVersion = VK_MAKE_VERSION(1, 0, 0),
            .pEngineName = "Simple Engine",
            .engineVersion = VK_MAKE_VERSION(1, 0, 0),
            .apiVersion = VK_API_VERSION_1_3};
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception during Vulkan instance creation: " << e.what() << std::endl;
        return false;
    }
}
