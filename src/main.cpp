#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include "VulkanPFNs.h"
#include "GlobalCreateInfos.h"
#include "Window.hpp"
#include "VulkanBackend.hpp"
#include "Device.hpp"

int main() {
    try {
        VulkanPFNs::fLoadVkGetInstanceProcAddr();
        VulkanPFNs::fLoadTrueGlobalVulkanFunctions();

        GlobalCreateInfos::fPopulateGlobalWindowCreateInfo();
        Window window(GlobalCreateInfos::gWindowWidth, GlobalCreateInfos::gWindowHeight, GlobalCreateInfos::gWindowName);

        GlobalCreateInfos::fPopulateGlobalInstanceCreateInfo();
        VulkanBackend vulkanBackend(&window, &GlobalCreateInfos::gInstanceCreateInfo);
        VulkanPFNs::fLoadVulkanFunctions();

        GlobalCreateInfos::fPopulateGlobalSelectedPhysicalDevice(VulkanPFNs::gInstanceInUse);
        GlobalCreateInfos::fPopulateGlobalLogicalDeviceCreateInfo();
        Device device(&vulkanBackend, GlobalCreateInfos::gSelectedPhysicalDevice, &GlobalCreateInfos::gLogicalDeviceCreateInfo);
    } catch(std::runtime_error const& runtimeError) {
        std::cout << "ERROR: " << runtimeError.what() << "\n";
    }

    return 0;
}