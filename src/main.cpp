#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>
#include "VulkanPFNs.h"
#include "GlobalCreateInfos.h"
#include "GlfwWindowWrapper.hpp"
#include "VulkanBackendWrapper.hpp"
#include "VulkanDevicesWrapper.hpp"
#include "VulkanSwapchainWrapper.hpp"

int main() {
    try {
        VulkanPFNs::fLoadVkGetInstanceProcAddr();
        VulkanPFNs::fLoadTrueGlobalVulkanFunctions();

        GlobalCreateInfos::fPopulateGlobalWindowCreateInfo();
        GlfwWindowWrapper glfwWindowWrapper(GlobalCreateInfos::gWindowWidth, GlobalCreateInfos::gWindowHeight, GlobalCreateInfos::gWindowName);

        GlobalCreateInfos::fPopulateGlobalInstanceCreateInfo();
        VulkanBackendWrapper vulkanBackendWrapper(&glfwWindowWrapper, &GlobalCreateInfos::gInstanceCreateInfo);
        VulkanPFNs::fLoadVulkanFunctions();

        GlobalCreateInfos::fPopulateGlobalSharedPhysicalLogicalDeviceInfo();
        GlobalCreateInfos::fPopulateGlobalSelectedPhysicalDevice(vulkanBackendWrapper.getVkInstance());
        GlobalCreateInfos::fPopulateGlobalLogicalDeviceCreateInfo();
        VulkanDevicesWrapper vulkanDevicesWrapper(&vulkanBackendWrapper, GlobalCreateInfos::gSelectedPhysicalDevice, &GlobalCreateInfos::gLogicalDeviceCreateInfo);
        
        VulkanSwapchainWrapper vulkanSwapchainWrapper(&vulkanDevicesWrapper);
        GlobalCreateInfos::fPopulateGlobalSwapchainKHRCreateInfo(vulkanDevicesWrapper.getVkPhysicalDevice(), vulkanSwapchainWrapper.getSurfaceKHR(), glfwWindowWrapper.getGlfwWindow());
        vulkanSwapchainWrapper.setSwapchainKHRCreateInfoAndArise(&GlobalCreateInfos::gSwapchainKHRCreateInfo);
    } catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cout << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}