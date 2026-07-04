#pragma once

#include <algorithm>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#ifndef VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#endif

#ifndef VULKAN_HPP_DISPATCH_LOADER_DYNAMIC
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#endif

#ifndef VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#define VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE
#endif

#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_hpp_macros.hpp>
#include <vulkan/vulkan_raii.hpp>

#include "platform.h"

#ifndef VK_EXT_ROBUSTNESS_2_EXTENSION_NAME
#define VK_EXT_ROBUSTNESS_2_EXTENSION_NAME "VK_EXT_robustness2"
#endif
#ifndef VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME
#define VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME "VK_KHR_dynamic_rendering_local_read"
#endif
#ifndef VK_EXT_SHADER_TILE_IMAGE_EXTENSION_NAME
#define VK_EXT_SHADER_TILE_IMAGE_EXTENSION_NAME "VK_EXT_shader_tile_image"
#endif

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;
    std::optional<uint32_t> computeFamily;
    std::optional<uint32_t> transferFamily;

    [[nodiscard]] bool isComplete() const
    {
        return graphicsFamily.has_value() && presentFamily.has_value() && computeFamily.has_value();
    }
};

struct SwapChainSupportDetails
{
    vk::SurfaceCapabilitiesKHR capabilities;
    std::vector<vk::SurfaceFormatKHR> formats;
    std::vector<vk::PresentModeKHR> presentModes;
};

class Renderer
{
public:
    explicit Renderer(Platform *platform);
    ~Renderer();

    bool Initialize(const std::string &appName, bool enableValidationLayers = true);
    void cleanup();

private:
    Platform *platform = nullptr;

    vk::raii::Context context;

    vk::raii::Instance instance = nullptr;
    vk::raii::DebugUtilsMessengerEXT debugMessenger = nullptr;

    vk::raii::PhysicalDevice physicalDevice = nullptr;
    vk::raii::Device device = nullptr;

    vk::raii::SurfaceKHR surface = nullptr;

    QueueFamilyIndices queueFamilyIndices;

    const std::vector<const char *> validationLayers = {
        "VK_LAYER_KHRONOS_validation"};

    const std::vector<const char *> requiredDeviceExtensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    // Optional device extensions
    const std::vector<const char *> optionalDeviceExtensions = {
        VK_KHR_DYNAMIC_RENDERING_EXTENSION_NAME,
        VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
        VK_KHR_DEPTH_STENCIL_RESOLVE_EXTENSION_NAME,
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME,
        // Robustness and safety
        VK_EXT_ROBUSTNESS_2_EXTENSION_NAME,
        // Tile/local memory friendly dynamic rendering readback
        VK_KHR_DYNAMIC_RENDERING_LOCAL_READ_EXTENSION_NAME,
        // Shader tile image for fast tile access
        VK_EXT_SHADER_TILE_IMAGE_EXTENSION_NAME,
        // Ray query support for ray-traced rendering
        VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
        VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
        VK_KHR_RAY_QUERY_EXTENSION_NAME};

    std::vector<const char *> deviceExtensions;

    bool initialized = false;

    bool createInstance(const std::string &appName, bool enableValidationLayers);
    bool setupDebugMessenger(bool enableValidationLayers);
    bool createSurface();
    bool pickPhysicalDevice();
    // bool createLogicalDevice();
    bool checkValidationLayerSupport() const;
    void addSupportedOptionalExtensions();

    QueueFamilyIndices findQueueFamilies(const vk::raii::PhysicalDevice &device);
    SwapChainSupportDetails querySwapChainSupport(const vk::raii::PhysicalDevice &device);
    bool checkDeviceExtensionSupport(const vk::raii::PhysicalDevice &device);
};
