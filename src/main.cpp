#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include "VulkanPFNs.h"
#include "GlobalCreateInfos.h"
#include "GlfwWindowWrapper.hpp"
#include "VulkanBackendWrapper.hpp"
#include "VulkanDevicesWrapper.hpp"

int main() {
    try {
        VulkanPFNs::fLoadVkGetInstanceProcAddr();
        VulkanPFNs::fLoadTrueGlobalVulkanFunctions();

        GlobalCreateInfos::fPopulateGlobalWindowCreateInfo();
        GlfwWindowWrapper window(GlobalCreateInfos::gWindowWidth, GlobalCreateInfos::gWindowHeight, GlobalCreateInfos::gWindowName);

        GlobalCreateInfos::fPopulateGlobalInstanceCreateInfo();
        VulkanBackendWrapper vulkanBackend(&window, &GlobalCreateInfos::gInstanceCreateInfo);
        VulkanPFNs::fLoadVulkanFunctions();

        GlobalCreateInfos::fPopulateGlobalSharedPhysicalLogicalDeviceInfo();
        GlobalCreateInfos::fPopulateGlobalSelectedPhysicalDevice(VulkanPFNs::gInstanceInUse);
        GlobalCreateInfos::fPopulateGlobalLogicalDeviceCreateInfo();
        VulkanDevicesWrapper device(&vulkanBackend, GlobalCreateInfos::gSelectedPhysicalDevice, &GlobalCreateInfos::gLogicalDeviceCreateInfo);
    } catch(std::runtime_error const& runtimeError) {
        std::cout << "ERROR: " << runtimeError.what() << "\n";
    }

    return 0;
}