
#include "PCH.h"

#define s *1000ll*1000ll*1000ll

// In *one* source file:
// #define VMA_IMPLEMENTATION

// If you don't like the `vma::` prefix:
//#define VMA_HPP_NAMESPACE <prefix>

// #include "vk_mem_alloc.hpp"

const std::string MODEL_PATH = "data/models/Spaceship.obj";
const std::string TEXTURE_PATH = "data/textures/Spaceship_color.jpg";

const uint32_t validationLayerCount = 1;
const char* validationLayers[validationLayerCount] = {"VK_LAYER_KHRONOS_validation"};

const uint32_t deviceExtensionCount = 1;
const char* deviceExtensions[validationLayerCount] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

//static VmaAllocator allocator;
//static VmaPool pool;
static VkBuffer buf;
//static VmaAllocation alloc;

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                    void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        std::cerr << "validation layer: " << pCallbackData->pMessage << '\n';
    }
    
    return VK_FALSE;
}

void
InitRenderer(Window* window, std::vector<const char*> glfwExtensions)
{
    window->renderer.physicalDevice = VK_NULL_HANDLE;
    window->renderer.window = window;
    
    bool res = enableValidationLayers && checkValidationLayerSupport();
    
    assert(res);
    
    VkApplicationInfo app_info = {VK_STRUCTURE_TYPE_APPLICATION_INFO, nullptr,           "APP_NAME", VK_MAKE_VERSION(1, 0, 0), "N O_ENGINE",
        VK_MAKE_VERSION(1, 0, 0),           VK_API_VERSION_1_2};
    
    VkInstanceCreateInfo inst_info = {
        VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO, nullptr, 0, &app_info, 0, nullptr, static_cast<uint32_t>(glfwExtensions.size()), glfwExtensions.data()};
    
    if (enableValidationLayers)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        
        populateDebugMessengerCreateInfo(createInfo);
        inst_info.pNext = &createInfo;
        inst_info.enabledLayerCount = validationLayerCount;
        inst_info.ppEnabledLayerNames = validationLayers;
    }
    
    VkResult result = vkCreateInstance(&inst_info, nullptr, &window->renderer.instance);
    
    assert(result == VK_SUCCESS);
    
    setupDebugMessenger(&window->renderer);
    
    createSurface(&window->renderer);
    
    pickPhysicalDevice(&window->renderer);
    
    createLogicalDevice(&window->renderer);
    
    createAllocator();
    
    createSwapChain(&window->renderer);
    
    createImageViews(&window->renderer);
    
    createRenderPass(&window->renderer);
    
    createDescriptorSetLayout(&window->renderer);
    
    createGraphicsPipeline(&window->renderer);
    
    createCommandPool(&window->renderer);
    
    createColorResources(&window->renderer);
    
    createDepthResources(&window->renderer);
    
    createFramebuffers(&window->renderer);
    
    createTextureImage(&window->renderer);
    
    createTextureImageView(&window->renderer);
    
    createTextureSampler(&window->renderer);
    
    loadModel(&window->renderer);
    
    createVertexBuffer(&window->renderer);
    
    createIndexBuffer(&window->renderer);
    
    createUniformBuffers(&window->renderer);
    
    createDescriptorPool(&window->renderer);
    
    createDescriptorSets(&window->renderer);
    
    createCommandBuffers(&window->renderer);
    
    createSyncObjects(&window->renderer);
}

void
TermRenderer(Renderer* renderer)
{
    cleanupSwapChain(renderer);
    
    vkDestroySampler(renderer->device, renderer->textureSampler, nullptr);
    vkDestroyImageView(renderer->device, renderer->textureImageView, nullptr);
    
    vkDestroyImage(renderer->device, renderer->textureImage, nullptr);
    vkFreeMemory(renderer->device, renderer->textureImageMemory, nullptr);
    
    vkDestroyDescriptorSetLayout(renderer->device, renderer->descriptorSetLayout, nullptr);
    
    vkDestroyBuffer(renderer->device, renderer->indexBuffer, nullptr);
    vkFreeMemory(renderer->device, renderer->indexBufferMemory, nullptr);
    
    vkDestroyBuffer(renderer->device, renderer->vertexBuffer, nullptr);
    vkFreeMemory(renderer->device, renderer->vertexBufferMemory, nullptr);
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) 
    {
        vkDestroySemaphore(renderer->device, renderer->renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(renderer->device, renderer->imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(renderer->device, renderer->inFlightFences[i], nullptr);
    }
    
    vkDestroyCommandPool(renderer->device, renderer->commandPool, nullptr);
    
    //vmaDestroyBuffer(allocator, buf, alloc);
    
    //vmaDestroyPool(allocator, pool);
    
    //vmaDestroyAllocator(allocator);
    
    vkDestroyDevice(renderer->device, nullptr);
    
    if (enableValidationLayers)
    {
        destroyDebugUtilsMessengerEXT(renderer->instance, renderer->debugMessenger, nullptr);
    }
    
    vkDestroySurfaceKHR(renderer->instance, renderer->surface, nullptr);
    
    vkDestroyInstance(renderer->instance, nullptr);
}

void
DrawFrame(Renderer* renderer)
{
    vkWaitForFences(renderer->device, 1, &renderer->inFlightFences[renderer->currentFrame], VK_TRUE, UINT64_MAX);
    
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(renderer->device, renderer->swapChain, UINT64_MAX, renderer->imageAvailableSemaphores[renderer->currentFrame], VK_NULL_HANDLE, &imageIndex);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR) 
    {
        RecreateSwapChain(renderer);
        return;
    }
    
    assert(result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR);
    
    updateUniformBuffer(renderer, imageIndex);
    
    if (renderer->imagesInFlight[imageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(renderer->device, 1, &renderer->imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
    }
    renderer->imagesInFlight[imageIndex] = renderer->inFlightFences[renderer->currentFrame];
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    
    VkSemaphore waitSemaphores[] = { renderer->imageAvailableSemaphores[renderer->currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &renderer->commandBuffers[imageIndex];
    
    VkSemaphore signalSemaphores[] = { renderer->renderFinishedSemaphores[renderer->currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    
    vkResetFences(renderer->device, 1, &renderer->inFlightFences[renderer->currentFrame]);
    
    result = vkQueueSubmit(renderer->graphicsQueue, 1, &submitInfo, renderer->inFlightFences[renderer->currentFrame]);
    
    assert(result == VK_SUCCESS);
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    
    VkSwapchainKHR swapChains[] = { renderer->swapChain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    
    presentInfo.pImageIndices = &imageIndex;
    
    result = vkQueuePresentKHR(renderer->presentQueue, &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || renderer->framebufferResized)
    {
        renderer->framebufferResized = false;
        RecreateSwapChain(renderer);
        return;
    }
    
    assert(result == VK_SUCCESS);
    
    renderer->currentFrame = (renderer->currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void
WaitIdle(Renderer* renderer)
{
    vkDeviceWaitIdle(renderer->device);
}

void
RecreateSwapChain(Renderer* renderer)
{
    int width = 0, height = 0;
    
    glfwGetFramebufferSize(renderer->window->glfw_window, &width, &height);
    
    while (width == 0 || height == 0) 
    {
        glfwGetFramebufferSize(renderer->window->glfw_window, &width, &height);
        glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(renderer->device);
    
    cleanupSwapChain(renderer);
    
    createSwapChain(renderer);
    createImageViews(renderer);
    createRenderPass(renderer);
    createGraphicsPipeline(renderer);
    createColorResources(renderer);
    createDepthResources(renderer);
    createFramebuffers(renderer);
    createUniformBuffers(renderer);
    createDescriptorPool(renderer);
    createDescriptorSets(renderer);
    createCommandBuffers(renderer);
}

// private

void
cleanupSwapChain(Renderer* renderer)
{
    vkDestroyImageView(renderer->device, renderer->colorImageView, nullptr);
    vkDestroyImage(renderer->device, renderer->colorImage, nullptr);
    vkFreeMemory(renderer->device, renderer->colorImageMemory, nullptr);
    
    vkDestroyImageView(renderer->device, renderer->depthImageView, nullptr);
    vkDestroyImage(renderer->device, renderer->depthImage, nullptr);
    vkFreeMemory(renderer->device, renderer->depthImageMemory, nullptr);
    
    for (size_t i = 0; i < renderer->swapChainFramebuffers.size(); i++)
    {
        vkDestroyFramebuffer(renderer->device, renderer->swapChainFramebuffers[i], nullptr);
    }
    
    vkFreeCommandBuffers(renderer->device, renderer->commandPool, static_cast<uint32_t>(renderer->commandBuffers.size()), renderer->commandBuffers.data());
    
    vkDestroyPipeline(renderer->device, renderer->graphicsPipeline, nullptr);
    vkDestroyPipelineLayout(renderer->device, renderer->pipelineLayout, nullptr);
    vkDestroyRenderPass(renderer->device, renderer->renderPass, nullptr);
    
    for (size_t i = 0; i < renderer->swapChainImageViews.size(); i++)
    {
        vkDestroyImageView(renderer->device, renderer->swapChainImageViews[i], nullptr);
    }
    
    vkDestroySwapchainKHR(renderer->device, renderer->swapChain, nullptr);
    
    for (size_t i = 0; i < renderer->swapChainImages.size(); i++)
    {
        vkDestroyBuffer(renderer->device, renderer->uniformBuffers[i], nullptr);
        vkFreeMemory(renderer->device, renderer->uniformBuffersMemory[i], nullptr);
    }
    
    vkDestroyDescriptorPool(renderer->device, renderer->descriptorPool, nullptr);
}

bool
isDeviceSuitable(Renderer* renderer, VkPhysicalDevice device)
{
    QueueFamilyIndices indices = findQueueFamilies(renderer->surface, device);
    
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    bool extensionsSupported = checkDeviceExtensionSupport(device);
    
    bool swapChainAdequate = false;
    
    if (extensionsSupported)
    {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(renderer->surface, device);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }
    
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
    
    return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ||
        deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU && 
        deviceFeatures.geometryShader && indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

void
pickPhysicalDevice(Renderer* renderer)
{
    uint32_t physicalDeviceCount = 0;
    vkEnumeratePhysicalDevices(renderer->instance, &physicalDeviceCount, nullptr);
    std::vector<VkPhysicalDevice> devices(physicalDeviceCount);
    vkEnumeratePhysicalDevices(renderer->instance, &physicalDeviceCount, devices.data());
    
    assert(devices.size() != 0);
    
    for (const auto& device : devices)
    {
        if (isDeviceSuitable(renderer, device))
        {
            renderer->physicalDevice = device;
            renderer->msaaSamples = getMaxUsableSampleCount(renderer->physicalDevice);
            break;
        }
    }
    
    assert(renderer->physicalDevice != VK_NULL_HANDLE);
}

void
checkExtensions(Renderer* renderer)
{
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(renderer->physicalDevice, nullptr, &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(renderer->physicalDevice, nullptr, &extensionCount, extensions.data());
    std::cout << "Vulkan Extensions:\n";
    for (const auto& extension : extensions)
    {
        std::cout << "\t" << extension.extensionName << '\n';
    }
}

bool
checkValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> avaliableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, avaliableLayers.data());
    
    for (const char* layerName : validationLayers)
    {
        for (const auto& layerProperties : avaliableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                return true;
            }
        }
    }
    
    return false;
}

VkResult
createDebugUtilsMessengerEXT(VkInstance instance,
                             const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                             const VkAllocationCallbacks* pAllocator,
                             VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    
    if (func != nullptr)
    {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else
    {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void
destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    
    if (func != nullptr)
    {
        func(instance, debugMessenger, pAllocator);
    }
}

void
populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {
        VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        nullptr,
        0,
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        debugCallback,
        nullptr};
}

void
setupDebugMessenger(Renderer* renderer)
{
    if (!enableValidationLayers)
        return;
    
    VkDebugUtilsMessengerCreateInfoEXT createInfo;
    populateDebugMessengerCreateInfo(createInfo);
    
    VkResult result = createDebugUtilsMessengerEXT(renderer->instance, &createInfo, nullptr, &renderer->debugMessenger);
    
    assert(result == VK_SUCCESS);
}

QueueFamilyIndices
findQueueFamilies(const VkSurfaceKHR& surface, VkPhysicalDevice device)
{
    QueueFamilyIndices familyIndices;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    for (uint32_t i = 0; i < queueFamilies.size(); i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            familyIndices.graphicsFamily = i;
        }
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        
        if (presentSupport)
        {
            familyIndices.presentFamily = i;
        }
        
        if (familyIndices.isComplete())
        {
            break;
        }
    }
    
    return familyIndices;
}

void
createLogicalDevice(Renderer* renderer)
{
    QueueFamilyIndices indices = findQueueFamilies(renderer->surface, renderer->physicalDevice);
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    
    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, nullptr, 0, queueFamily, 1, &queuePriority};
        
        queueCreateInfos.push_back(queueCreateInfo);
    }
    
    VkPhysicalDeviceFeatures deviceFeatures{};
    deviceFeatures.samplerAnisotropy = VK_TRUE;
    deviceFeatures.sampleRateShading = VK_TRUE;
    
    VkDeviceCreateInfo createInfo = {VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        nullptr,
        0,
        static_cast<uint32_t>(queueCreateInfos.size()),
        queueCreateInfos.data(),
        0,
        nullptr,
        deviceExtensionCount,
        deviceExtensions,
        &deviceFeatures};
    
    if (enableValidationLayers)
    {
        createInfo.enabledLayerCount = validationLayerCount;
        createInfo.ppEnabledLayerNames = validationLayers;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
    }
    
    VkResult result = vkCreateDevice(renderer->physicalDevice, &createInfo, nullptr, &renderer->device);
    
    assert(result == VK_SUCCESS);
    
    vkGetDeviceQueue(renderer->device, indices.graphicsFamily.value(), 0, &renderer->graphicsQueue);
    vkGetDeviceQueue(renderer->device, indices.presentFamily.value(), 0, &renderer->presentQueue);
}

void
createSurface(Renderer* renderer)
{
    VkResult result = glfwCreateWindowSurface(renderer->instance, renderer->window->glfw_window, nullptr, &renderer->surface);
    
    assert(result == VK_SUCCESS);
}

bool
checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    
    std::set<std::string> requiredExtensions(deviceExtensions, deviceExtensions + deviceExtensionCount);
    
    for (const auto& extension : availableExtensions) 
    {
        requiredExtensions.erase(extension.extensionName);
    }
    
    return requiredExtensions.empty();
}

SwapChainSupportDetails 
querySwapChainSupport(const VkSurfaceKHR& surface, VkPhysicalDevice device)
{
    SwapChainSupportDetails details;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
    
    if (formatCount != 0) 
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
    
    if (presentModeCount != 0) 
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }
    
    return details;
}

VkSurfaceFormatKHR
chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) 
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
        {
            return availableFormat;
        }
    }
    
    return availableFormats[0];
}

VkPresentModeKHR
chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) 
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) 
        {
            return availablePresentMode;
        }
    }
    
    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D
chooseSwapExtent(GLFWwindow* glfw_window, const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX) 
    {
        return capabilities.currentExtent;
    }
    else 
    {
        int width, height;
        glfwGetFramebufferSize(glfw_window, &width, &height);
        
        VkExtent2D actualExtent = 
        {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };
        
        actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
        actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));
        
        return actualExtent;
    }
}

void
createSwapChain(Renderer* renderer)
{
    SwapChainSupportDetails swapChainSupport = querySwapChainSupport(renderer->surface, renderer->physicalDevice);
    
    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(renderer->window->glfw_window, swapChainSupport.capabilities);
    
    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) 
    {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }
    
    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = renderer->surface;
    
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    
    QueueFamilyIndices indices = findQueueFamilies(renderer->surface, renderer->physicalDevice);
    uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
    
    if (indices.graphicsFamily != indices.presentFamily) 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else 
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    
    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    
    VkResult result = vkCreateSwapchainKHR(renderer->device, &createInfo, nullptr, &renderer->swapChain);
    
    assert(result == VK_SUCCESS);
    
    vkGetSwapchainImagesKHR(renderer->device, renderer->swapChain, &imageCount, nullptr);
    renderer->swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(renderer->device, renderer->swapChain, &imageCount, renderer->swapChainImages.data());
    
    renderer->swapChainImageFormat = surfaceFormat.format;
    renderer->swapChainExtent = extent;
}

void
createImageViews(Renderer* renderer)
{
    renderer->swapChainImageViews.resize(renderer->swapChainImages.size());
    
    for (uint32_t i = 0; i < renderer->swapChainImages.size(); i++)
    {
        renderer->swapChainImageViews[i] = createImageView(renderer->device, renderer->swapChainImages[i], renderer->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
    }
}

std::vector<char>
readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    
    assert(file.is_open());
    
    uint32_t fileSize = static_cast<uint32_t>(file.tellg());
    std::vector<char> buffer(fileSize);
    
    file.seekg(0);
    file.read(buffer.data(), fileSize);
    
    file.close();
    
    return buffer;
}

VkShaderModule
createShaderModule(VkDevice& device, const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
    
    VkShaderModule shaderModule = NULL;
    
    VkResult result = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
    
    assert(result == VK_SUCCESS);
    
    return shaderModule;
}

void
createRenderPass(Renderer* renderer)
{
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = renderer->swapChainImageFormat;
    colorAttachment.samples = renderer->msaaSamples;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkAttachmentDescription depthAttachment{};
    depthAttachment.format = findDepthFormat(renderer);
    depthAttachment.samples = renderer->msaaSamples;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkAttachmentDescription colorAttachmentResolve{};
    colorAttachmentResolve.format = renderer->swapChainImageFormat;
    colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    
    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkAttachmentReference depthAttachmentRef{};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkAttachmentReference colorAttachmentResolveRef{};
    colorAttachmentResolveRef.attachment = 2;
    colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;
    subpass.pResolveAttachments = &colorAttachmentResolveRef;
    
    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    
    std::array<VkAttachmentDescription, 3> attachments = { colorAttachment, depthAttachment, colorAttachmentResolve };
    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments = attachments.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;
    
    VkResult result = vkCreateRenderPass(renderer->device, &renderPassInfo, nullptr, &renderer->renderPass);
    
    assert(result == VK_SUCCESS);
}

void
createGraphicsPipeline(Renderer* renderer)
{
    auto vertShaderCode = readFile("shaders/vert.spv");
    auto fragShaderCode = readFile("shaders/frag.spv");
    
    VkShaderModule vertShaderModule = createShaderModule(renderer->device, vertShaderCode);
    VkShaderModule fragShaderModule = createShaderModule(renderer->device, fragShaderCode);
    
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";
    
    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };
    
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    
    auto bindingDescription = getBindingDescription();
    auto attributeDescriptions = getAttributeDescriptions();
    
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    
    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float)renderer->swapChainExtent.width;
    viewport.height = (float)renderer->swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    
    VkRect2D scissor{};
    scissor.offset = { 0, 0 };
    scissor.extent = renderer->swapChainExtent;
    
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;
    
    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_TRUE;
    multisampling.minSampleShading = 0.2f;
    multisampling.rasterizationSamples = renderer->msaaSamples;
    
    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;
    
    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    
    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &renderer->descriptorSetLayout;
    
    VkResult result = vkCreatePipelineLayout(renderer->device, &pipelineLayoutInfo, nullptr, &renderer->pipelineLayout);
    
    assert(result == VK_SUCCESS);
    
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = renderer->pipelineLayout;
    pipelineInfo.renderPass = renderer->renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    
    result = vkCreateGraphicsPipelines(renderer->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &renderer->graphicsPipeline);
    
    assert(result == VK_SUCCESS);
    
    vkDestroyShaderModule(renderer->device, fragShaderModule, nullptr);
    vkDestroyShaderModule(renderer->device, vertShaderModule, nullptr);
}

void
createFramebuffers(Renderer* renderer)
{
    VkResult result;
    
    renderer->swapChainFramebuffers.resize(renderer->swapChainImageViews.size());
    
    for (size_t i = 0; i < renderer->swapChainImageViews.size(); i++)
    {
        std::array<VkImageView, 3> attachments =
        {
            renderer->colorImageView,
            renderer->depthImageView,
            renderer->swapChainImageViews[i]
        };
        
        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderer->renderPass;
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = renderer->swapChainExtent.width;
        framebufferInfo.height = renderer->swapChainExtent.height;
        framebufferInfo.layers = 1;
        
        result = vkCreateFramebuffer(renderer->device, &framebufferInfo, nullptr, &renderer->swapChainFramebuffers[i]);
        
        assert(result == VK_SUCCESS);
    }
}

void
createCommandPool(Renderer* renderer)
{
    QueueFamilyIndices queueFamilyIndices = findQueueFamilies(renderer->surface, renderer->physicalDevice);
    
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();
    
    VkResult result = vkCreateCommandPool(renderer->device, &poolInfo, nullptr, &renderer->commandPool);
    
    assert(result == VK_SUCCESS);
}

void
createCommandBuffers(Renderer* renderer)
{
    renderer->commandBuffers.resize(renderer->swapChainFramebuffers.size());
    
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = renderer->commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t)renderer->commandBuffers.size();
    
    VkResult result = vkAllocateCommandBuffers(renderer->device, &allocInfo, renderer->commandBuffers.data());
    
    assert(result == VK_SUCCESS);
    
    for (size_t i = 0; i < renderer->commandBuffers.size(); i++)
    {
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        result = vkBeginCommandBuffer(renderer->commandBuffers[i], &beginInfo);
        
        assert(result == VK_SUCCESS);
        
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderer->renderPass;
        renderPassInfo.framebuffer = renderer->swapChainFramebuffers[i];
        renderPassInfo.renderArea.offset = { 0, 0 };
        renderPassInfo.renderArea.extent = renderer->swapChainExtent;
        
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.0f, 0.0f, 0.0f, 1.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();
        
        vkCmdBeginRenderPass(renderer->commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        
        vkCmdBindPipeline(renderer->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->graphicsPipeline);
        
        VkBuffer vertexBuffers[] = { renderer->vertexBuffer };
        VkDeviceSize offsets[] = { 0 };
        vkCmdBindVertexBuffers(renderer->commandBuffers[i], 0, 1, vertexBuffers, offsets);
        
        vkCmdBindIndexBuffer(renderer->commandBuffers[i], renderer->indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        
        vkCmdBindDescriptorSets(renderer->commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, renderer->pipelineLayout, 0, 1, &renderer->descriptorSets[i], 0, nullptr);
        
        vkCmdDrawIndexed(renderer->commandBuffers[i], static_cast<uint32_t>(renderer->indices.size()), 1, 0, 0, 0);
        
        vkCmdEndRenderPass(renderer->commandBuffers[i]);
        
        result = vkEndCommandBuffer(renderer->commandBuffers[i]);
        
        assert(result == VK_SUCCESS);
    }
}

void
createSyncObjects(Renderer* renderer)
{
    renderer->imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderer->renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    renderer->inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
    renderer->imagesInFlight.resize(renderer->swapChainImages.size(), VK_NULL_HANDLE);
    
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    
    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    
    VkResult result;
    
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        result = vkCreateSemaphore(renderer->device, &semaphoreInfo, nullptr, &renderer->imageAvailableSemaphores[i]);
        assert(result == VK_SUCCESS);
        result = vkCreateSemaphore(renderer->device, &semaphoreInfo, nullptr, &renderer->renderFinishedSemaphores[i]);
        assert(result == VK_SUCCESS);
        result = vkCreateFence(renderer->device, &fenceInfo, nullptr, &renderer->inFlightFences[i]);
        assert(result == VK_SUCCESS);
    }
}

void
createBuffer(Renderer* renderer, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult result = vkCreateBuffer(renderer->device, &bufferInfo, nullptr, &buffer);
    
    assert(result == VK_SUCCESS);
    
    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(renderer->device, buffer, &memRequirements);
    
    //
    
    //VmaAllocationCreateInfo allocInfo = {};
    //allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
    
    //VmaAllocation allocation;
    //result = vmaCreateBuffer(allocator, &bufferInfo, &allocInfo, &buffer, &allocation, nullptr);
    
    //
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(renderer->physicalDevice, memRequirements.memoryTypeBits, properties);
    
    result = vkAllocateMemory(renderer->device, &allocInfo, nullptr, &bufferMemory);
    
    //
    
    assert(result == VK_SUCCESS);
    
    vkBindBufferMemory(renderer->device, buffer, bufferMemory, 0);
}

void
copyBuffer(Renderer* renderer, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(renderer);
    
    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
    
    endSingleTimeCommands(renderer, commandBuffer);
}

void
createVertexBuffer(Renderer* renderer)
{
    VkDeviceSize bufferSize = sizeof(renderer->vertices[0]) * renderer->vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    
    void* data;
    vkMapMemory(renderer->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, renderer->vertices.data(), (size_t)bufferSize);
    vkUnmapMemory(renderer->device, stagingBufferMemory);
    
    createBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderer->vertexBuffer, renderer->vertexBufferMemory);
    
    copyBuffer(renderer, stagingBuffer, renderer->vertexBuffer, bufferSize);
    
    vkDestroyBuffer(renderer->device, stagingBuffer, nullptr);
    vkFreeMemory(renderer->device, stagingBufferMemory, nullptr);
}

void
createIndexBuffer(Renderer* renderer)
{
    VkDeviceSize bufferSize = sizeof(renderer->indices[0]) * renderer->indices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    createBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);
    
    void* data;
    vkMapMemory(renderer->device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, renderer->indices.data(), (size_t)bufferSize);
    vkUnmapMemory(renderer->device, stagingBufferMemory);
    
    createBuffer(renderer, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderer->indexBuffer, renderer->indexBufferMemory);
    
    copyBuffer(renderer, stagingBuffer, renderer->indexBuffer, bufferSize);
    
    vkDestroyBuffer(renderer->device, stagingBuffer, nullptr);
    vkFreeMemory(renderer->device, stagingBufferMemory, nullptr);
}

uint32_t
findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
    
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ( (typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties )
        {
            return i;
        }
    }
    
    assert(false);
    
    return NULL;
}

void
createDescriptorSetLayout(Renderer* renderer)
{
    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    uboLayoutBinding.binding = 0;
    uboLayoutBinding.descriptorCount = 1;
    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboLayoutBinding.pImmutableSamplers = nullptr;
    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    
    VkDescriptorSetLayoutBinding samplerLayoutBinding{};
    samplerLayoutBinding.binding = 1;
    samplerLayoutBinding.descriptorCount = 1;
    samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerLayoutBinding.pImmutableSamplers = nullptr;
    samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    
    std::array<VkDescriptorSetLayoutBinding, 2> bindings = { uboLayoutBinding, samplerLayoutBinding };
    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    layoutInfo.pBindings = bindings.data();
    
    VkResult result = vkCreateDescriptorSetLayout(renderer->device, &layoutInfo, nullptr, &renderer->descriptorSetLayout);
    
    assert(result == VK_SUCCESS);
}

void
createUniformBuffers(Renderer* renderer)
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);
    
    renderer->uniformBuffers.resize(renderer->swapChainImages.size());
    renderer->uniformBuffersMemory.resize(renderer->swapChainImages.size());
    
    for (size_t i = 0; i < renderer->swapChainImages.size(); i++)
    {
        createBuffer(renderer, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, renderer->uniformBuffers[i], renderer->uniformBuffersMemory[i]);
    }
}

void
updateUniformBuffer(Renderer* renderer, uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();
    
    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();
    
    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ubo.view = renderer->window->camera.view;
    //glm::lookAt(glm::vec3(10.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    
#if 1
    ubo.proj = glm::perspective(glm::radians(80.0f), renderer->swapChainExtent.width / (float)renderer->swapChainExtent.height, 0.1f, 100.0f);
#else
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    const float aspect = (float)width / (float)height;
    
    float ratio_size_per_depth = atan(glm::radians(45.0f));
    
    auto distance = glm::length(glm::vec3(0.0f, 0.0f, 0.0f) - glm::vec3(1.0f, 1.0f, 0.0f));
    
    float size_y = 10.0f;
    float size_x = 10.0f * aspect;
    
    ubo.proj = glm::ortho(-1.0f * aspect, 1.0f * aspect, -1.0f, 1.0f, -10.0f, 20.0f);// -20.0f, 10.0f);
    
    float orthScale = 0.20f;
    ubo.proj = glm::scale(ubo.proj, glm::vec3(orthScale, orthScale, 1.0f));
#endif
    
    ubo.proj[1][1] *= -1;
    
    void* data;
    vkMapMemory(renderer->device, renderer->uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(renderer->device, renderer->uniformBuffersMemory[currentImage]);
}

void
createDescriptorPool(Renderer* renderer)
{
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(renderer->swapChainImages.size());
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = static_cast<uint32_t>(renderer->swapChainImages.size());
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(renderer->swapChainImages.size());
    
    VkResult result = vkCreateDescriptorPool(renderer->device, &poolInfo, nullptr, &renderer->descriptorPool);
    
    assert(result == VK_SUCCESS);
}

void
createDescriptorSets(Renderer* renderer)
{
    std::vector<VkDescriptorSetLayout> layouts(renderer->swapChainImages.size(), renderer->descriptorSetLayout);
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = renderer->descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(renderer->swapChainImages.size());
    allocInfo.pSetLayouts = layouts.data();
    
    renderer->descriptorSets.resize(renderer->swapChainImages.size());
    
    VkResult result = vkAllocateDescriptorSets(renderer->device, &allocInfo, renderer->descriptorSets.data());
    
    assert(result == VK_SUCCESS);
    
    for (size_t i = 0; i < renderer->swapChainImages.size(); i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = renderer->uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);
        
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = renderer->textureImageView;
        imageInfo.sampler = renderer->textureSampler;
        
        std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
        
        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[0].dstSet = renderer->descriptorSets[i];
        descriptorWrites[0].dstBinding = 0;
        descriptorWrites[0].dstArrayElement = 0;
        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        descriptorWrites[0].descriptorCount = 1;
        descriptorWrites[0].pBufferInfo = &bufferInfo;
        
        descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorWrites[1].dstSet = renderer->descriptorSets[i];
        descriptorWrites[1].dstBinding = 1;
        descriptorWrites[1].dstArrayElement = 0;
        descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorWrites[1].descriptorCount = 1;
        descriptorWrites[1].pImageInfo = &imageInfo;
        
        vkUpdateDescriptorSets(renderer->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    }
}

void
createTextureImage(Renderer* renderer)
{
    int texWidth, texHeight, texChannels;
    stbi_uc* pixels = stbi_load(TEXTURE_PATH.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    VkDeviceSize imageSize = texWidth * texHeight * 4;
    
    assert(pixels);
    
    createBuffer(renderer, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, renderer->stagingBuffer, renderer->stagingBufferMemory);
    
    void* data;
    vkMapMemory(renderer->device, renderer->stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(renderer->device, renderer->stagingBufferMemory);
    
    stbi_image_free(pixels);
    
    renderer->mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(texWidth, texHeight)))) + 1;
    
    createImage(renderer, texWidth, texHeight, renderer->mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderer->textureImage, renderer->textureImageMemory);
    
    transitionImageLayout(renderer, renderer->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, renderer->mipLevels);
    copyBufferToImage(renderer, renderer->stagingBuffer, renderer->textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    //transitionImageLayout(textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels);
    
    vkDestroyBuffer(renderer->device, renderer->stagingBuffer, nullptr);
    vkFreeMemory(renderer->device, renderer->stagingBufferMemory, nullptr);
    
    generateMipmaps(renderer, renderer->textureImage, VK_FORMAT_R8G8B8A8_SRGB, texWidth, texHeight, renderer->mipLevels);
}

void
createImage(Renderer* renderer, uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory)
{
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = width;
    imageInfo.extent.height = height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = 1;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.samples = numSamples;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    VkResult result = vkCreateImage(renderer->device, &imageInfo, nullptr, &image);
    assert(result == VK_SUCCESS);
    
    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(renderer->device, image, &memRequirements);
    
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(renderer->physicalDevice, memRequirements.memoryTypeBits, properties);
    
    result = vkAllocateMemory(renderer->device, &allocInfo, nullptr, &imageMemory);
    
    assert(result == VK_SUCCESS);
    
    vkBindImageMemory(renderer->device, image, imageMemory, 0);
}

VkCommandBuffer
beginSingleTimeCommands(Renderer* renderer)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = renderer->commandPool;
    allocInfo.commandBufferCount = 1;
    
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(renderer->device, &allocInfo, &commandBuffer);
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
    return commandBuffer;
}

void
endSingleTimeCommands(Renderer* renderer, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    
    vkQueueSubmit(renderer->graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(renderer->graphicsQueue);
    
    vkFreeCommandBuffers(renderer->device, renderer->commandPool, 1, &commandBuffer);
}

void
transitionImageLayout(Renderer* renderer, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(renderer);
    
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = mipLevels;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    
    VkPipelineStageFlags sourceStage;
    VkPipelineStageFlags destinationStage;
    
    if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) 
    {
        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    }
    else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) 
    {
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    }
    else 
    {
        assert(false);
    }
    
    vkCmdPipelineBarrier(
                         commandBuffer,
                         sourceStage, destinationStage,
                         0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier
                         );
    
    endSingleTimeCommands(renderer, commandBuffer);
}

void
copyBufferToImage(Renderer* renderer, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(renderer);
    
    VkBufferImageCopy region{};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;
    
    region.imageOffset = { 0, 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };
    
    vkCmdCopyBufferToImage(
                           commandBuffer,
                           buffer,
                           image,
                           VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1,
                           &region
                           );
    
    endSingleTimeCommands(renderer, commandBuffer);
}

void
createTextureImageView(Renderer* renderer)
{
    renderer->textureImageView = createImageView(renderer->device, renderer->textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

VkImageView
createImageView(VkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = mipLevels;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;
    
    VkImageView imageView = NULL;
    
    VkResult result = vkCreateImageView(device, &viewInfo, nullptr, &imageView);
    
    assert(result == VK_SUCCESS);
    
    return imageView;
}

void
createTextureSampler(Renderer* renderer)
{
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(renderer->physicalDevice, &properties);
    
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    //samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    //samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = static_cast<float>(renderer->mipLevels);
    samplerInfo.mipLodBias = 0.0f;
    
    VkResult result = vkCreateSampler(renderer->device, &samplerInfo, nullptr, &renderer->textureSampler);
    
    assert(result == VK_SUCCESS);
}

void
createDepthResources(Renderer* renderer)
{
    VkFormat depthFormat = findDepthFormat(renderer);
    
    createImage(renderer, renderer->swapChainExtent.width, renderer->swapChainExtent.height, 1, renderer->msaaSamples, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderer->depthImage, renderer->depthImageMemory);
    renderer->depthImageView = createImageView(renderer->device, renderer->depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT, 1);
}

VkFormat findSupportedFormat(VkPhysicalDevice& physicalDevice, const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) 
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);
        
        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) 
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) 
        {
            return format;
        }
    }
    
    assert(false);
    return VK_FORMAT_UNDEFINED;
}

VkFormat
findDepthFormat(Renderer* renderer)
{
    return findSupportedFormat(renderer->physicalDevice,
                               { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
                               );
}

bool
hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void
loadModel(Renderer* renderer)
{
    //tinyobj::attrib_t attrib;
    //std::vector<tinyobj::shape_t> shapes;
    //std::vector<tinyobj::material_t> materials;
    //std::string warn, err;
    
    //bool result = tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str());
    
    Model model;
    
    LoadModel(&model, MODEL_PATH.c_str());
    
    //assert(result);
    
    //std::unordered_map<Vertex, uint32_t> uniqueVertices{};
    
    for (const auto& mesh : model.meshes)
    {
        for (size_t i = 0; i < mesh.vertices.size(); i++)
        {
            renderer->vertices.push_back({ mesh.vertices[i].pos, mesh.vertices[i].color, mesh.vertices[i].texCoord });
        }
        
        for (const auto& index : mesh.indices)
        {
            renderer->indices.push_back(index);
        }
    }
    
    /*for (const auto& shape : shapes) 
    {
        for (const auto& index : shape.mesh.indices) 
        {
            Vertex vertex{};

            vertex.pos = 
            {
                attrib.vertices[3 * index.vertex_index + 0],
                attrib.vertices[3 * index.vertex_index + 1],
                attrib.vertices[3 * index.vertex_index + 2]
            };

            vertex.texCoord = 
            {
                attrib.texcoords[2 * index.texcoord_index + 0],
                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
            };

            vertex.color = { 1.0f, 1.0f, 1.0f };

            if (uniqueVertices.count(vertex) == 0) 
            {
                uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                vertices.push_back(vertex);
            }

            indices.push_back(uniqueVertices[vertex]);
        }
    }*/
}

void
generateMipmaps(Renderer* renderer, VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels)
{
    // Check if image format supports linear blitting
    VkFormatProperties formatProperties;
    vkGetPhysicalDeviceFormatProperties(renderer->physicalDevice, imageFormat, &formatProperties);
    
    assert(formatProperties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT);
    
    VkCommandBuffer commandBuffer = beginSingleTimeCommands(renderer);
    
    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.image = image;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;
    barrier.subresourceRange.levelCount = 1;
    
    int32_t mipWidth = texWidth;
    int32_t mipHeight = texHeight;
    
    for (uint32_t i = 1; i < mipLevels; i++) {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        
        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
        
        VkImageBlit blit{};
        blit.srcOffsets[0] = { 0, 0, 0 };
        blit.srcOffsets[1] = { mipWidth, mipHeight, 1 };
        blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.srcSubresource.mipLevel = i - 1;
        blit.srcSubresource.baseArrayLayer = 0;
        blit.srcSubresource.layerCount = 1;
        blit.dstOffsets[0] = { 0, 0, 0 };
        blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
        blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blit.dstSubresource.mipLevel = i;
        blit.dstSubresource.baseArrayLayer = 0;
        blit.dstSubresource.layerCount = 1;
        
        vkCmdBlitImage(commandBuffer,
                       image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                       image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                       1, &blit,
                       VK_FILTER_LINEAR);
        
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        
        vkCmdPipelineBarrier(commandBuffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);
        
        if (mipWidth > 1) mipWidth /= 2;
        if (mipHeight > 1) mipHeight /= 2;
    }
    
    barrier.subresourceRange.baseMipLevel = mipLevels - 1;
    barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
    
    vkCmdPipelineBarrier(commandBuffer,
                         VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                         0, nullptr,
                         0, nullptr,
                         1, &barrier);
    
    endSingleTimeCommands(renderer, commandBuffer);
}

VkSampleCountFlagBits
getMaxUsableSampleCount(VkPhysicalDevice& physicalDevice)
{
    VkPhysicalDeviceProperties physicalDeviceProperties;
    vkGetPhysicalDeviceProperties(physicalDevice, &physicalDeviceProperties);
    
    VkSampleCountFlags counts = physicalDeviceProperties.limits.framebufferColorSampleCounts & physicalDeviceProperties.limits.framebufferDepthSampleCounts;
    if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
    if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
    if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
    if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
    if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
    if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }
    
    return VK_SAMPLE_COUNT_1_BIT;
}

void
createColorResources(Renderer* renderer)
{
    VkFormat colorFormat = renderer->swapChainImageFormat;
    
    createImage(renderer, renderer->swapChainExtent.width, renderer->swapChainExtent.height, 1, renderer->msaaSamples, colorFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, renderer->colorImage, renderer->colorImageMemory);
    renderer->colorImageView = createImageView(renderer->device, renderer->colorImage, colorFormat, VK_IMAGE_ASPECT_COLOR_BIT, 1);
}

void
createAllocator()
{
    //VmaAllocatorCreateInfo allocator_info{};
    //allocator_info.physicalDevice = physicalDevice;
    //allocator_info.device = device;
    
    ////if (VK_KHR_dedicated_allocation_enabled)
    ////{
    ////  allocatorInfo.flags |= VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT;
    ////}
    
    //vmaCreateAllocator(&allocator_info, &allocator);
    
    //VmaPoolCreateInfo poolCreateInfo = {};
    //poolCreateInfo.memoryTypeIndex = 0;
    //poolCreateInfo.blockSize = 1024ull * 1024 * 1024;
    //poolCreateInfo.maxBlockCount = 2;
    //
    //vmaCreatePool(allocator, &poolCreateInfo, &pool);
    
    //// Allocate a buffer out of it.
    //VkBufferCreateInfo bufCreateInfo = { VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
    //bufCreateInfo.size = 2048ull * 1024 * 1024;
    //bufCreateInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    
    //VmaAllocationCreateInfo allocCreateInfo = {};
    //allocCreateInfo.pool = pool;
    
    //VmaAllocationInfo allocInfo;
    //vmaCreateBuffer(allocator, &bufCreateInfo, &allocCreateInfo, &buf, &alloc, &allocInfo);
}
