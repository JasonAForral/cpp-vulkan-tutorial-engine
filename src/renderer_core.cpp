#include "renderer.h"

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
            extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
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
