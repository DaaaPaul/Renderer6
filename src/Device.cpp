#include "Device.hpp"
#include <iostream>
#include <climits>

Device::Device(VulkanBackend* givenVulkanBackend, VkPhysicalDevice const& GIVEN_PHYSICAL_DEVICE, VkDeviceCreateInfo const* GIVEN_LOGICAL_DEVICE_CREATE_INFO) : 
    vulkanBackend{ givenVulkanBackend },
    physicalDevice{ GIVEN_PHYSICAL_DEVICE },
    LOGICAL_DEVICE_CREATE_INFO{ GIVEN_LOGICAL_DEVICE_CREATE_INFO } {

    std::cout << "SET VULKAN BACKEND CREATE PARAMETERS:\n";
    
}

Device::~Device() {
    std::cout << "Destroying Device...\n";

    std::cout << "Destroyed Device\n";
}

void Device::arise() {
    std::cout << "Creating Device...\n";

    std::cout << "Created Device\n";
}