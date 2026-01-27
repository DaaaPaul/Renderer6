#include "VulkanDevicesWrapper.hpp"
#include <iostream>
#include <climits>

VulkanDevicesWrapper::VulkanDevicesWrapper(VulkanBackendWrapper* givenVulkanBackendWrapper, VkPhysicalDevice const& GIVEN_PHYSICAL_DEVICE, VkDeviceCreateInfo const* GIVEN_LOGICAL_DEVICE_CREATE_INFO) : 
    mVulkanBackend{ givenVulkanBackendWrapper },
    mPhysicalDevice{ GIVEN_PHYSICAL_DEVICE },
    mLOGICAL_DEVICE_CREATE_INFO{ GIVEN_LOGICAL_DEVICE_CREATE_INFO } {

    std::cout << "SET VULKAN BACKEND CREATE PARAMETERS:\n";
    
}

VulkanDevicesWrapper::~VulkanDevicesWrapper() {
    std::cout << "Destroying VulkanDevicesWrapper...\n";

    std::cout << "Destroyed VulkanDevicesWrapper\n";
}

void VulkanDevicesWrapper::arise() {
    std::cout << "Creating VulkanDevicesWrapper...\n";

    std::cout << "Created VulkanDevicesWrapper\n";
}