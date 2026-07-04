
#include "renderer.h"
#include <array>
#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>

// void Renderer::WaitIdle()
// {
//     std::vector<vk::Fence> allFenses;
//     allFenses.reserve(inFlightFences.size)
// }

QueueFamilyIndices Renderer::findQueueFamilies(const vk::raii::PhysicalDevice &device)
{
    QueueFamilyIndices indices;

    std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();

    for (uint32_t i = 0; i < queueFamilies.size(); ++i)
    {
        const auto &qf = queueFamilies[i];

        if ((qf.queueFlags & vk::QueueFlagBits::eGraphics) && !indices.graphicsFamily.has_value())
        {
            indices.graphicsFamily = i;
        }
        if ((qf.queueFlags & vk::QueueFlagBits::eCompute) && !indices.computeFamily.has_value())
        {
            indices.computeFamily = i;
        }
        if (!indices.presentFamily.has_value() && device.getSurfaceSupportKHR(i, *surface))
        {
            indices.presentFamily = i;
        }
        if ((qf.queueFlags & vk::QueueFlagBits::eTransfer) && !indices.transferFamily.has_value())
        {
            indices.transferFamily = i;
        }

        if (indices.isComplete() && indices.transferFamily.has_value())
        {
            break;
        }
    }

    if (!indices.transferFamily.has_value() && indices.graphicsFamily.has_value())
    {
        indices.transferFamily = indices.graphicsFamily;
    }

    return indices;
}

SwapChainSupportDetails Renderer::querySwapChainSupport(const vk::raii::PhysicalDevice &device)
{
    SwapChainSupportDetails details;

    details.capabilities = device.getSurfaceCapabilitiesKHR(*surface);

    details.formats = device.getSurfaceFormatsKHR(*surface);

    details.presentModes = device.getSurfacePresentModesKHR(*surface);

    return details;
}
