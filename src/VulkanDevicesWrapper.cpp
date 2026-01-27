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

    std::cout << "\t-Device features:\n";
    VulkanDevicesWrapper::printEnabledFeaturesInVkFeatureStruct(mLOGICAL_DEVICE_CREATE_INFO->pNext, "VkPhysicalDeviceFeatures2");
    VulkanDevicesWrapper::printEnabledFeaturesInVkFeatureStruct(reinterpret_cast<VkPhysicalDeviceFeatures2 const*>(mLOGICAL_DEVICE_CREATE_INFO->pNext)->pNext, "VkPhysicalDeviceSynchronization2Features");
    VulkanDevicesWrapper::printEnabledFeaturesInVkFeatureStruct(reinterpret_cast<VkPhysicalDeviceSynchronization2Features const*>(reinterpret_cast<VkPhysicalDeviceFeatures2 const*>(mLOGICAL_DEVICE_CREATE_INFO->pNext)->pNext)->pNext, "VkPhysicalDeviceDynamicRenderingFeatures");
    VulkanDevicesWrapper::printEnabledFeaturesInVkFeatureStruct(reinterpret_cast<VkPhysicalDeviceDynamicRenderingFeatures const*>(reinterpret_cast<VkPhysicalDeviceSynchronization2Features const*>(reinterpret_cast<VkPhysicalDeviceFeatures2 const*>(mLOGICAL_DEVICE_CREATE_INFO->pNext)->pNext)->pNext)->pNext, "VkPhysicalDeviceExtendedDynamicState2FeaturesEXT");

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

void VulkanDevicesWrapper::printEnabledFeaturesInVkFeatureStruct(void const* VK_FEATURE_STRUCT, const char* featureName) {
    const uint16_t FIRST_VKBOOL32_OFFSET = 16; // assuming sType is bytes 0-3 and pNext is bytes 8-15
    unsigned char const* BYTE_POINTER = reinterpret_cast<unsigned char const*>(VK_FEATURE_STRUCT);
    BYTE_POINTER += FIRST_VKBOOL32_OFFSET;
    VkBool32 const* VK_BOOLS_POINTER = reinterpret_cast<VkBool32 const*>(BYTE_POINTER);

    for(int i = 0; (*VK_BOOLS_POINTER == VK_TRUE) || (*VK_BOOLS_POINTER == VK_FALSE); i++) {
        if(*VK_BOOLS_POINTER == VK_TRUE) {
            std::cout << "\t\t-Feature " << i << " from the top down (zero-indexed) is enabled in the struct " << featureName << "\n";
        }
        VK_BOOLS_POINTER++;
    }
}