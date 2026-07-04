#include "renderer.h"

#include <map>
#include <set>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallbackVkHpp(
    vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    [[maybe_unused]] vk::DebugUtilsMessageTypeFlagsEXT messageType,
    const vk::DebugUtilsMessengerCallbackDataEXT *pCallbackData,
    [[maybe_unused]] void *pUserData)
{
    if (messageSeverity >= vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
    {
        std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;
    }
    else
    {
        std::cout << "Validation layer: " << pCallbackData->pMessage << std::endl;
    }
    return vk::False;
}

Renderer::Renderer(Platform *platform) : platform(platform)
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

    LOGI("instance created...");

    if (!setupDebugMessenger(enableValidationLayers))
    {
        std::cerr << "Failed to setup debug messenger" << std::endl;
        return false;
    }

    LOGI("debug messenger created...");

    if (!createSurface())
    {
        std::cerr << "Failed to create surface" << std::endl;
        return false;
    }

    LOGI("surface created...");

    if (!pickPhysicalDevice())
    {
        std::cerr << "Failed to pick a physical device" << std::endl;
        return false;
    }

    return true;
}

void Renderer::cleanup()
{

    if (!initialized)
    {
        return;
    }

    std::cout << "Starting renderer cleanup..." << std::endl;

    // Wait for the device to be idle before cleaning up
    try
    {
        // WaitIdle();
    }
    catch (...)
    {
    }

    surface = nullptr;

    initialized = false;
    std::cout << "Renderer cleanup completed." << std::endl;
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

        std::vector<const char *> extensions;

#if defined(PLATFORM_DESKTOP)
        std::cout << "Platform Desktop" << std::endl;
        uint32_t glfwExtensionCount = 0;
        const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        extensions.insert(extensions.end(), glfwExtensions, glfwExtensions + glfwExtensionCount);
#endif

        if (enableValidationLayers)
        {
            extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        vk::InstanceCreateInfo createInfo{
            .pApplicationInfo = &appInfo,
            .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
            .ppEnabledExtensionNames = extensions.data()};

        vk::ValidationFeaturesEXT validationFeatures{};
        std::vector<vk::ValidationFeatureEnableEXT> enabledValidationFeatures;

        if (enableValidationLayers)
        {
            if (!checkValidationLayerSupport())
            {
                std::cerr << "Validation layers requested, but not available" << std::endl;
                return false;
            }

            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            validationFeatures.enabledValidationFeatureCount = static_cast<uint32_t>(enabledValidationFeatures.size());
            validationFeatures.pEnabledValidationFeatures = enabledValidationFeatures.data();

            createInfo.pNext = &validationFeatures;
        }

        instance = vk::raii::Instance(context, createInfo);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to create instance:" << e.what() << std::endl;
        return false;
    }
}

bool Renderer::setupDebugMessenger(bool enableValidationLayers)
{
    if (!enableValidationLayers)
    {
        return true;
    }

    try
    {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo{
            .messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                               vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            .messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                           vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                           vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance};

#if defined(__ANDROID__)
        createInfo.pfnUserCallback = &debugCallbackVkRaii;
#else
        createInfo.pfnUserCallback = &debugCallbackVkHpp;
#endif

        debugMessenger = vk::raii::DebugUtilsMessengerEXT(instance, createInfo);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to setup debug messenger: " << e.what() << std::endl;
        return false;
    }
}

bool Renderer::createSurface()
{
    try
    {
        VkSurfaceKHR _surface;
        if (!platform->CreateVulkanSurface(*instance, &_surface))
        {
            std::cerr << "Failed platform request to create surface" << std::endl;
            return false;
        }
        surface = vk::raii::SurfaceKHR(instance, _surface);
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to create surface: " << e.what() << std::endl;
        return false;
    }
}

bool Renderer::pickPhysicalDevice()
{
    try
    {
        std::vector<vk::raii::PhysicalDevice> devices = instance.enumeratePhysicalDevices();

        if (devices.empty())
        {
            std::cerr << "Failed to find GPUs with Vulkan Support" << std::endl;
            return false;
        }

        std::multimap<int, vk::raii::PhysicalDevice> suitableDevices;

        for (auto &_device : devices)
        {
            vk::PhysicalDeviceProperties deviceProperties = _device.getProperties();

            std::cout << "Checking device: " << deviceProperties.deviceName
                      << " (Type: " << vk::to_string(deviceProperties.deviceType) << "}" << std::endl;

            bool supportsVulkan1_3 = deviceProperties.apiVersion >= VK_API_VERSION_1_3;
            if (!supportsVulkan1_3)
            {
                std::cout << "  - Does not support Vulkan 1.3" << std::endl;
                continue;
            }

            QueueFamilyIndices indices = findQueueFamilies(_device);
            bool supportGraphics = indices.isComplete();
            if (!supportGraphics)
            {
                std::cout << "  - Missing required queue families" << std::endl;
                continue;
            }

            SwapChainSupportDetails swapChainSupport = querySwapChainSupport(_device);
            bool swapchainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
            if (!swapchainAdequate)
            {
                std::cout << "  - Inadequate swap chain support" << std::endl;
                continue;
            }

            auto features = _device.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan13Features>();
            bool supportsRequiredFeatures = features.get<vk::PhysicalDeviceVulkan13Features>().dynamicRendering;
            if (!supportsRequiredFeatures)
            {
                std::cout << "  - Does not support required features (dynamicRendering)" << std::endl;
                continue;
            }

            int score = 0;

            if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
            {
                score += 1000;
                std::cout << "  - Discrete GPU: +1000 points" << std::endl;
            }
            else if (deviceProperties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
            {
                score += 100;
                std::cout << "  - Integrated GPU: +100 points" << std::endl;
            }

            vk::PhysicalDeviceMemoryProperties memProperties = _device.getMemoryProperties();
            for (uint32_t i = 0; i < memProperties.memoryHeapCount; ++i)
            {
                if (memProperties.memoryHeaps[i].flags & vk::MemoryHeapFlagBits::eDeviceLocal)
                {
                    score += static_cast<int>(memProperties.memoryHeaps[i].size / (1024 * 1024 * 1024));
                    break;
                }
            }

            std::cout << "  - Device is suitable with score: " << score << std::endl;
            suitableDevices.emplace(score, _device);
        }

        if (!suitableDevices.empty())
        {
            physicalDevice = suitableDevices.rbegin()->second;
            vk::PhysicalDeviceProperties deviceProperties = physicalDevice.getProperties();

            std::cout << "Selected device: " << deviceProperties.deviceName
                      << " (Type: " << vk::to_string(deviceProperties.deviceType)
                      << ", Score: " << suitableDevices.rbegin()->first << ")" << std::endl;

            queueFamilyIndices = findQueueFamilies(physicalDevice);

            addSupportedOptionalExtensions();
            return true;
        }
        std::cerr << "Failed to find a suitable GPU. Make sure your GPU supports Vulkan and has the required extensions." << std::endl;
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to pick physical device: " << e.what() << std::endl;
        return false;
    }
}

bool Renderer::checkValidationLayerSupport() const
{
    std::vector<vk::LayerProperties> availableLayers = context.enumerateInstanceLayerProperties();

    for (const char *layerName : validationLayers)
    {
        bool layerFound = false;

        for (const auto &layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }
    return true;
}

void Renderer::addSupportedOptionalExtensions()
{
    try
    {
        auto availableExtensions = physicalDevice.enumerateDeviceExtensionProperties();

        std::set<std::string> avail;
        for (const auto &e : availableExtensions)
        {
            avail.insert(e.extensionName);
        }

        for (const auto &optionalExt : optionalDeviceExtensions)
        {
            deviceExtensions.push_back(optionalExt);
            std::cout << "Adding optional extension: " << optionalExt << std::endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Warning: Failed to add optional extensions: " << e.what() << std::endl;
    }
}
