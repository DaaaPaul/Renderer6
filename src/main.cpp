#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "VulkanPFNs.h"
#include "Window.hpp"
#include "VulkanBackend.hpp"

int main() {
    try {
        loadVkGetInstanceProcAddr();
        loadTrueGlobalVulkanFunctions();

        Window window(800, 600, "Renderer6");

        const VkApplicationInfo APP_INFO{
            .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
            .pNext = nullptr,
            .pApplicationName = nullptr,
            .applicationVersion = 0,
            .pEngineName = nullptr,
            .engineVersion = 0,
            .apiVersion = VK_API_VERSION_1_3,
        };
        const std::vector<const char*> ENABLED_LOADER_LAYERS{ "VK_LAYER_KHRONOS_validation" };
        const std::vector<const char*> ENABLED_INSTANCE_EXTENSIONS(VulkanBackend::getGlfwWindowExtensions());
        const VkInstanceCreateInfo INSTANCE_CREATE_INFO{
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = &APP_INFO,
            .enabledLayerCount = STATIC_CAST_VECTOR_SIZE(ENABLED_LOADER_LAYERS),
            .ppEnabledLayerNames = ENABLED_LOADER_LAYERS.data(),
            .enabledExtensionCount = STATIC_CAST_VECTOR_SIZE(ENABLED_INSTANCE_EXTENSIONS),
            .ppEnabledExtensionNames = ENABLED_INSTANCE_EXTENSIONS.data(),
        };
        VulkanBackend vulkanBackend(INSTANCE_CREATE_INFO);

        loadVulkanFunctions();
    } catch(std::runtime_error const& runtimeError) {
        std::cout << "ERROR: " << runtimeError.what() << '\n';
    }

    return 0;
}