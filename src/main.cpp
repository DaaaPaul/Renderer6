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

        GlobalCreateInfos::gAppInfo = VkApplicationInfo{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = nullptr,
            .applicationVersion = 0,
            .pEngineName = nullptr,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_3,
        };
        GlobalCreateInfos::gEnabledLoaderLayers = { "VK_LAYER_KHRONOS_validation" };
        GlobalCreateInfos::gEnabledInstanceExtensions = VulkanBackend::getGlfwWindowExtensions();

        for(std::string const& s : GlobalCreateInfos::gEnabledInstanceExtensions) {
            std::cout << s << '\n';
        }

        GlobalCreateInfos::gInstanceCreateInfo = VkInstanceCreateInfo{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &GlobalCreateInfos::gAppInfo,
            .enabledLayerCount = static_cast<uint32_t>(GlobalCreateInfos::gEnabledLoaderLayers.size()),
            .ppEnabledLayerNames = GlobalCreateInfos::gEnabledLoaderLayers.data(),
            .enabledExtensionCount = static_cast<uint32_t>(GlobalCreateInfos::gEnabledInstanceExtensions.size()),
            .ppEnabledExtensionNames = GlobalCreateInfos::gEnabledInstanceExtensions.data(),
        };
        VulkanBackend vulkanBackend(&GlobalCreateInfos::gInstanceCreateInfo);

        VulkanPFNs::fLoadVulkanFunctions();
    } catch(std::runtime_error const& runtimeError) {
        std::cout << "ERROR: " << runtimeError.what() << "\n";
    }

    return 0;
}