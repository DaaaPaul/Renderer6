#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include "VulkanPFNs.h"
#include "Window.hpp"
#include "VulkanBackend.hpp"
#include "GlobalCreateInfos.h"

void manuallyInitializeGlfw() {
    glfwInit();
}

void populateGlobalCreateInfos() {
    // window stuff
    GlobalCreateInfos::gWindowWidth = 800;
    GlobalCreateInfos::gWindowHeight = 600;
    GlobalCreateInfos::gWindowName = "Renderer6";

    // vulkan instance stuff
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
    VkInstanceCreateFlags instanceCreateFlags = 0;
    #ifdef __APPLE__
    GlobalCreateInfos::gEnabledInstanceExtensions.push_back("VK_KHR_portability_enumeration");
    instanceCreateFlags += VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
    #endif
    GlobalCreateInfos::gInstanceCreateInfo = VkInstanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = instanceCreateFlags,
        .pApplicationInfo = &GlobalCreateInfos::gAppInfo,
        .enabledLayerCount = static_cast<uint32_t>(GlobalCreateInfos::gEnabledLoaderLayers.size()),
        .ppEnabledLayerNames = GlobalCreateInfos::gEnabledLoaderLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(GlobalCreateInfos::gEnabledInstanceExtensions.size()),
        .ppEnabledExtensionNames = GlobalCreateInfos::gEnabledInstanceExtensions.data(),
    };
}

int main() {
    try {
        VulkanPFNs::fLoadVkGetInstanceProcAddr();
        VulkanPFNs::fLoadTrueGlobalVulkanFunctions();
        manuallyInitializeGlfw();
        populateGlobalCreateInfos();

        Window window(GlobalCreateInfos::gWindowWidth, GlobalCreateInfos::gWindowHeight, GlobalCreateInfos::gWindowName);

        VulkanBackend vulkanBackend(&window, &GlobalCreateInfos::gInstanceCreateInfo);
        VulkanPFNs::fLoadVulkanFunctions();


    } catch(std::runtime_error const& runtimeError) {
        std::cout << "ERROR: " << runtimeError.what() << "\n";
    }

    return 0;
}