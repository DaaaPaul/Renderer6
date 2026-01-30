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

        GlfwWindowWrapper glfwWindowWrapper(GlfwWindowWrapper::getConstructParameters());

        VulkanBackendWrapper vulkanBackendWrapper(&glfwWindowWrapper, VulkanBackendWrapper::getConstructParameters());
        VulkanPFNs::gInstanceInUse = vulkanBackendWrapper.mInstance;
        VulkanPFNs::fLoadVulkanFunctions();

        VulkanDevicesWrapper vulkanDevicesWrapper(&vulkanBackendWrapper, VulkanDevicesWrapper::getConstructParameters(vulkanBackendWrapper.mInstance));
        
        VulkanSwapchainWrapper vulkanSwapchainWrapper(&vulkanDevicesWrapper);
        GlobalCreateInfos::fPopulateGlobalSwapchainKHRCreateInfo(vulkanDevicesWrapper.mPhysicalDevice, vulkanSwapchainWrapper.getSurfaceKHR(), glfwWindowWrapper.mGlfwWindow);
        vulkanSwapchainWrapper.setSwapchainKHRCreateInfoAndArise(&GlobalCreateInfos::gSwapchainKHRCreateInfo);
    } catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cout << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}