#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "VulkanPFNs.h"
#include "Window.hpp"
#include "VulkanBackend.hpp"
#include "GlobalCreateInfos.h"

int main() {
    try {
        VulkanPFNs::fLoadVkGetInstanceProcAddr();
        VulkanPFNs::fLoadTrueGlobalVulkanFunctions();

        Window window(800, 600, "Renderer6");

        GlobalCreateInfos::appInfo = VkApplicationInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = nullptr,
            .applicationVersion = 0,
            .pEngineName = nullptr,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_3,
        };
        GlobalCreateInfos::enabledLoaderLayers = { "VK_LAYER_KHRONOS_validation" };
        GlobalCreateInfos::enabledInstanceExtensions = VulkanBackend::getGlfwWindowExtensions();
        GlobalCreateInfos::instanceCreateInfo = VkInstanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &GlobalCreateInfos::appInfo,
            .enabledLayerCount = static_cast<uint32_t>(GlobalCreateInfos::enabledLoaderLayers.size()),
            .ppEnabledLayerNames = GlobalCreateInfos::enabledLoaderLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(GlobalCreateInfos::enabledInstanceExtensions.size()),
            .ppEnabledExtensionNames = GlobalCreateInfos::enabledInstanceExtensions.data(),
        };
        VulkanBackend vulkanBackend(GlobalCreateInfos::instanceCreateInfo);

        VulkanPFNs::fLoadVulkanFunctions();
    } catch(std::runtime_error const& runtimeError) {
        std::cout << "ERROR: " << runtimeError.what() << "\n";
    }

    return 0;
}