#include "VulkanDevicesWrapper.hpp"
#include "VulkanPFNs.h"
#include "GlobalCreateInfos.h"
#include <iostream>

VulkanDevicesWrapper::VulkanDevicesWrapper(VulkanBackendWrapper* givenVulkanBackendWrapper, VkPhysicalDevice const& GIVEN_PHYSICAL_DEVICE, VkDeviceCreateInfo const* GIVEN_LOGICAL_DEVICE_CREATE_INFO) : 
    mVulkanBackendWrapper{ givenVulkanBackendWrapper },
    mPhysicalDevice{ GIVEN_PHYSICAL_DEVICE },
    mLogicalDevice{},
    mLOGICAL_DEVICE_CREATE_INFO{ GIVEN_LOGICAL_DEVICE_CREATE_INFO } {

    std::cout << "SET VULKAN DEVICES CREATE PARAMETERS:\n";

    VkPhysicalDeviceProperties physicalDeviceProperties{};
    VulkanPFNs::gpVkGetPhysicalDeviceProperties(mPhysicalDevice, &physicalDeviceProperties);
    std::cout << "\t-Physical device: " << physicalDeviceProperties.deviceName << "\n";

    std::cout << "\t-Device extensions:\n";
    for(int i = 0; i < mLOGICAL_DEVICE_CREATE_INFO->enabledExtensionCount; i++) {
        std::cout << "\t\t-" << mLOGICAL_DEVICE_CREATE_INFO->ppEnabledExtensionNames[i] << '\n';
    }

    std::cout << "\t-Device features (too lazy to list):\n";

    std::cout << "\t-Device queues:\n";
    for(int i = 0; i < mLOGICAL_DEVICE_CREATE_INFO->queueCreateInfoCount; i++) {
        std::cout << "\t\t-Queue family index:" << mLOGICAL_DEVICE_CREATE_INFO->pQueueCreateInfos[i].queueFamilyIndex << '\n';
        std::cout << "\t\t-Queue count:" << mLOGICAL_DEVICE_CREATE_INFO->pQueueCreateInfos[i].queueCount << '\n';
        for(int j = 0; j < mLOGICAL_DEVICE_CREATE_INFO->pQueueCreateInfos[i].queueCount; j++) {
            std::cout << "\t\t-Queue priorities:" << mLOGICAL_DEVICE_CREATE_INFO->pQueueCreateInfos[i].pQueuePriorities[j] << '\n';
        }
    }

    arise();
}

VulkanDevicesWrapper::~VulkanDevicesWrapper() {
    std::cout << "Destroying VulkanDevicesWrapper...\n";

    VulkanPFNs::gpVkDestroyDevice(mLogicalDevice, nullptr);

    std::cout << "Destroyed VulkanDevicesWrapper\n";
}

void VulkanDevicesWrapper::arise() {
    std::cout << "Creating VulkanDevicesWrapper...\n";

    CHECK_VK_SUCCESS(VulkanPFNs::gpVkCreateDevice(mPhysicalDevice, mLOGICAL_DEVICE_CREATE_INFO, nullptr, &mLogicalDevice))

    std::cout << "Created VulkanDevicesWrapper\n";
}