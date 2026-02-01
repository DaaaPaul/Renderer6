#include "VulkanDevicesWrapper.hpp"
#include "VulkanPFNs.h"
#include <iostream>
#include <array>

VulkanDevicesWrapper::VulkanDevicesWrapper(VulkanBackendWrapper* givenVulkanBackendWrapper, VulkanDevicesWrapperConstructParameters const& GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS) :
    mVulkanBackendWrapper{ givenVulkanBackendWrapper },
    mPhysicalDevice{ GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS.selectedPhysicalDevice },
    mLogicalDevice{},
    mParameters{ GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS } {

    // reroute pointers
    mParameters.deviceEnabledDynamicRenderingFeatures.pNext = &mParameters.deviceEnabledExtendedDynamicStateFeatures;
    mParameters.deviceEnabledSyncFeatures.pNext = &mParameters.deviceEnabledDynamicRenderingFeatures;
    mParameters.enabledDeviceFeatures.pNext = &mParameters.deviceEnabledSyncFeatures;

    mParameters.logicalDeviceCreateInfo.pNext = &mParameters.enabledDeviceFeatures;

    mParameters.deviceQueueFamilyCreateInfos[0].pQueuePriorities = mParameters.deviceQueueFamilyQueuePriorities[0].data();

    mParameters.logicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(mParameters.deviceQueueFamilyCreateInfos.size());
    mParameters.logicalDeviceCreateInfo.pQueueCreateInfos = mParameters.deviceQueueFamilyCreateInfos.data();
    mParameters.logicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(mParameters.enabledDeviceExtensions.size());
    mParameters.logicalDeviceCreateInfo.ppEnabledExtensionNames = mParameters.enabledDeviceExtensions.data();

    // print parameters
    std::cout << "SET VULKAN DEVICES CREATE PARAMETERS:\n";

    {
        VkPhysicalDeviceProperties physicalDeviceProperties{};
        VulkanPFNs::gpVkGetPhysicalDeviceProperties(mParameters.selectedPhysicalDevice, &physicalDeviceProperties);
        std::cout << "\t-Physical device: " << physicalDeviceProperties.deviceName << "\n";
    }

    {
        std::cout << "\t-Device extensions:\n";
        for (int i = 0; i < mParameters.logicalDeviceCreateInfo.enabledExtensionCount; i++) {
            std::cout << "\t\t-" << mParameters.logicalDeviceCreateInfo.ppEnabledExtensionNames[i] << '\n';
        }
    }

    {
        auto printEnabledFeaturesInVkFeatureStruct = [](void const* VK_FEATURE_STRUCT, const char* FEATURE_NAME)-> void {
            const uint16_t FIRST_VKBOOL32_OFFSET = offsetof(VkPhysicalDeviceFeatures2, pNext) + 8;
            unsigned char const* BYTE_POINTER = reinterpret_cast<unsigned char const*>(VK_FEATURE_STRUCT) + FIRST_VKBOOL32_OFFSET;
            VkBool32 const* VK_BOOLS_POINTER = reinterpret_cast<VkBool32 const*>(BYTE_POINTER);

            for (int i = 0; (*VK_BOOLS_POINTER == VK_TRUE) || (*VK_BOOLS_POINTER == VK_FALSE); i++) {
                if (*VK_BOOLS_POINTER == VK_TRUE) {
                    std::cout << "\t\t-Feature " << i << " from the top down (zero-indexed) is enabled in the struct " << FEATURE_NAME << "\n";
                }
                VK_BOOLS_POINTER++;
            }
            };
        std::cout << "\t-Device features:\n";
        printEnabledFeaturesInVkFeatureStruct(mParameters.logicalDeviceCreateInfo.pNext, "VkPhysicalDeviceFeatures2");
        printEnabledFeaturesInVkFeatureStruct(reinterpret_cast<VkPhysicalDeviceFeatures2 const*>(mParameters.logicalDeviceCreateInfo.pNext)->pNext, "VkPhysicalDeviceSynchronization2Features");
        printEnabledFeaturesInVkFeatureStruct(reinterpret_cast<VkPhysicalDeviceSynchronization2Features const*>(reinterpret_cast<VkPhysicalDeviceFeatures2 const*>(mParameters.logicalDeviceCreateInfo.pNext)->pNext)->pNext, "VkPhysicalDeviceDynamicRenderingFeatures");
        printEnabledFeaturesInVkFeatureStruct(reinterpret_cast<VkPhysicalDeviceDynamicRenderingFeatures const*>(reinterpret_cast<VkPhysicalDeviceSynchronization2Features const*>(reinterpret_cast<VkPhysicalDeviceFeatures2 const*>(mParameters.logicalDeviceCreateInfo.pNext)->pNext)->pNext)->pNext, "VkPhysicalDeviceExtendedDynamicState2FeaturesEXT");
    }

    {
        std::cout << "\t-Device queues:\n";
        for (int i = 0; i < mParameters.logicalDeviceCreateInfo.queueCreateInfoCount; i++) {
            std::cout << "\t\t-Queue family index:" << mParameters.logicalDeviceCreateInfo.pQueueCreateInfos[i].queueFamilyIndex << '\n';
            std::cout << "\t\t-Queue count:" << mParameters.logicalDeviceCreateInfo.pQueueCreateInfos[i].queueCount << '\n';
            for (int j = 0; j < mParameters.logicalDeviceCreateInfo.pQueueCreateInfos[i].queueCount; j++) {
                std::cout << "\t\t-Queue priorities:" << mParameters.logicalDeviceCreateInfo.pQueueCreateInfos[i].pQueuePriorities[j] << '\n';
            }
        }
    }

    // construct the logical device
    std::cout << "Creating VulkanDevicesWrapper...\n";

	CHECK_VK_SUCCESS(
		VulkanPFNs::gpVkCreateDevice(mPhysicalDevice, &mParameters.logicalDeviceCreateInfo, nullptr, &mLogicalDevice),
		"Failed to create logical device"
	)
	mGraphicsFamilyQueues.resize(mParameters.logicalDeviceCreateInfo.pQueueCreateInfos[0].queueCount, VK_NULL_HANDLE);
	for(int i = 0; i < mGraphicsFamilyQueues.size(); i++) {
		VulkanPFNs::gpVkGetDeviceQueue(mLogicalDevice, mParameters.logicalDeviceCreateInfo.pQueueCreateInfos[0].queueFamilyIndex, i, &mGraphicsFamilyQueues[i]);
	}

    std::cout << "Created VulkanDevicesWrapper\n";
}

VulkanDevicesWrapper::~VulkanDevicesWrapper() {
    std::cout << "Destroying VulkanDevicesWrapper...\n";

    VulkanPFNs::gpVkDestroyDevice(mLogicalDevice, nullptr);

    std::cout << "Destroyed VulkanDevicesWrapper\n";
}

[[nodiscard]] VulkanDevicesWrapper::VulkanDevicesWrapperConstructParameters VulkanDevicesWrapper::getConstructParameters(VkInstance instance) {
    VulkanDevicesWrapperConstructParameters returnValue{};

    auto setupEnabledFeatures = [&returnValue]() -> void {
        returnValue.deviceEnabledExtendedDynamicStateFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
            .extendedDynamicState2 = true
        };
        returnValue.deviceEnabledDynamicRenderingFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
            .pNext = nullptr, // reroute needed
            .dynamicRendering = true
        };
        returnValue.deviceEnabledSyncFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
            .pNext = nullptr, // reroute needed
            .synchronization2 = true
        };
        returnValue.enabledDeviceFeatures = {
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
            .pNext = nullptr, // reroute needed
            .features = {
                .samplerAnisotropy = true
            }
        };
    };
    setupEnabledFeatures();

    auto setupEnabledExtensions = [&returnValue]() -> void {
        returnValue.enabledDeviceExtensions = {
            "VK_KHR_swapchain",
            "VK_KHR_synchronization2",
            "VK_KHR_spirv_1_4",
            #ifdef __APPLE__
            "VK_KHR_portability_subset"
            #endif
        };
    };
    setupEnabledExtensions();

    auto setupQueueFamilyCreateInfo = [&returnValue]() -> void {
        returnValue.deviceQueueFamilyQueuePriorities = {
            {0.5f}
        };

        returnValue.deviceQueueFamilyCreateInfos = {
            VkDeviceQueueCreateInfo{
                .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .queueFamilyIndex = UINT32_MAX, // warning value: this must be set non-absurdly later
                .queueCount = 1,
                .pQueuePriorities = nullptr // reroute needed
            }
        };
    };
    setupQueueFamilyCreateInfo();

    auto selectPhysicalDevice = [&returnValue](VkInstance instance) -> void {
        uint32_t physicalDeviceCount{};
        CHECK_VK_SUCCESS(
            VulkanPFNs::gpVkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr),
            "Failed to enumerate physical devices on your instance"
        );
        std::vector<VkPhysicalDevice> systemPhysicalDevices(physicalDeviceCount);
        CHECK_VK_SUCCESS(
            VulkanPFNs::gpVkEnumeratePhysicalDevices(instance, &physicalDeviceCount, systemPhysicalDevices.data()),
            "Failed to enumerate physical devices on your instance"
        );

        // weed out the downs
        for (int i = 0; i < systemPhysicalDevices.size(); i++) {
            VkPhysicalDeviceProperties physicalDeviceProperties{};
            VulkanPFNs::gpVkGetPhysicalDeviceProperties(systemPhysicalDevices[i], &physicalDeviceProperties);

            // api version check
            if (!(physicalDeviceProperties.apiVersion >= VK_API_VERSION_1_3)) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for too low API version of below " << UINT32_TO_VK_API_VERSION_CSTR(VK_API_VERSION_1_3) << "\n";
                continue;
            }

            // has graphics queue family with enough queues check
            uint32_t physicalDeviceQueueFamilyCount{};
            VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(systemPhysicalDevices[i], &physicalDeviceQueueFamilyCount, nullptr);
            std::vector<VkQueueFamilyProperties> physicalDeviceQueueFamilyProperties(physicalDeviceQueueFamilyCount);
            VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(systemPhysicalDevices[i], &physicalDeviceQueueFamilyCount, physicalDeviceQueueFamilyProperties.data());

            bool foundGraphicsQueueFamilyWithEnoughQueues = false;
            for (int i = 0; i < physicalDeviceQueueFamilyCount; i++) {
                if ((physicalDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                    (physicalDeviceQueueFamilyProperties[i].queueCount >= returnValue.deviceQueueFamilyCreateInfos[0].queueCount)) {
                    foundGraphicsQueueFamilyWithEnoughQueues = true;
                }
            }

            if (!foundGraphicsQueueFamilyWithEnoughQueues) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for not having a graphics queue family with eough queues" << "\n";
                continue;
            }

            // extensions check
            uint32_t physicalDeviceExtensionsCount{};
            VulkanPFNs::gpVkEnumerateDeviceExtensionProperties(systemPhysicalDevices[i], nullptr, &physicalDeviceExtensionsCount, nullptr);
            std::vector<VkExtensionProperties> physicalDeviceExtensions(physicalDeviceExtensionsCount);
            VulkanPFNs::gpVkEnumerateDeviceExtensionProperties(systemPhysicalDevices[i], nullptr, &physicalDeviceExtensionsCount, physicalDeviceExtensions.data());

            std::vector<std::string> physicalDeviceExtensionNames{};
            for (VkExtensionProperties const& physicalDeviceExtension : physicalDeviceExtensions) {
                physicalDeviceExtensionNames.push_back(physicalDeviceExtension.extensionName);
            }
            std::vector<std::string> logicalDeviceExtensionNames{};
            for (int i = 0; i < returnValue.enabledDeviceExtensions.size(); i++) {
                logicalDeviceExtensionNames.push_back(returnValue.enabledDeviceExtensions[i]);
            }

            if (!Common::containsAll(physicalDeviceExtensionNames, logicalDeviceExtensionNames)) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for not having the device extensions this application needs" << "\n";
                continue;
            }

            // features check
            VkPhysicalDeviceExtendedDynamicState2FeaturesEXT physicalDeviceExtendedDynamicStateFeaturesStatus{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT
            };
            VkPhysicalDeviceDynamicRenderingFeatures physicalDeviceDynamicRenderingFeaturesStatus{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
                .pNext = &physicalDeviceExtendedDynamicStateFeaturesStatus
            };
            VkPhysicalDeviceSynchronization2Features physicalDeviceSyncFeaturesStatus{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
                .pNext = &physicalDeviceDynamicRenderingFeaturesStatus
            };
            VkPhysicalDeviceFeatures2 physicalDeviceFeaturesStatus{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
                .pNext = &physicalDeviceSyncFeaturesStatus
            };
            VulkanPFNs::gpVkGetPhysicalDeviceFeatures2(systemPhysicalDevices[i], &physicalDeviceFeaturesStatus);

            if (!(physicalDeviceFeaturesStatus.features.samplerAnisotropy && physicalDeviceSyncFeaturesStatus.synchronization2 && physicalDeviceDynamicRenderingFeaturesStatus.dynamicRendering && physicalDeviceExtendedDynamicStateFeaturesStatus.extendedDynamicState2)) {
                systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
                std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for not having the device features this application needs" << "\n";
                continue;
            }
        }

        if (systemPhysicalDevices.empty()) {
            throw std::runtime_error("There is no suitable GPU on your computer for this application");
        }

        // perform nice-to-have checks on the remaining physical devices
        // index 0 : are you a discrete GPU?
        std::vector<std::array<uint16_t, 1>> niceToHavesBySystemPhysicalDevice(systemPhysicalDevices.size(), { 0 });
        for (int i = 0; i < systemPhysicalDevices.size(); i++) {
            VkPhysicalDeviceProperties physicalDeviceProperties{};
            VulkanPFNs::gpVkGetPhysicalDeviceProperties(systemPhysicalDevices[i], &physicalDeviceProperties);

            if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                niceToHavesBySystemPhysicalDevice[i][0] = 1;
            }
        }

        std::vector<uint16_t> niceToHaveSumsBySystemPhysicalDevice(systemPhysicalDevices.size(), 0);
        for (int i = 0; i < systemPhysicalDevices.size(); i++) {
            for (uint16_t const& niceToHaveStatus : niceToHavesBySystemPhysicalDevice[i]) {
                niceToHaveSumsBySystemPhysicalDevice[i] += niceToHaveStatus;
            }
        }

        size_t selectedPhysicalDeviceIndex = std::distance(niceToHaveSumsBySystemPhysicalDevice.begin(), std::max_element(niceToHaveSumsBySystemPhysicalDevice.begin(), niceToHaveSumsBySystemPhysicalDevice.end()));
        VkPhysicalDevice selectedPhysicalDevice = systemPhysicalDevices[selectedPhysicalDeviceIndex];
        VkPhysicalDeviceProperties selectedPhysicalDeviceProperties{};
        VulkanPFNs::gpVkGetPhysicalDeviceProperties(selectedPhysicalDevice, &selectedPhysicalDeviceProperties);
        std::cout << "SELECTED PHYSICAL DEVICE: " << selectedPhysicalDeviceProperties.deviceName << "\n";
        std::cout << "DISCRETE GPU? " << ((niceToHavesBySystemPhysicalDevice[selectedPhysicalDeviceIndex][0]) ? "Yes\n" : "No\n");

        returnValue.selectedPhysicalDevice = systemPhysicalDevices[selectedPhysicalDeviceIndex];
    };
    selectPhysicalDevice(instance);
    
    returnValue.deviceQueueFamilyCreateInfos[0].queueFamilyIndex = VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(returnValue.selectedPhysicalDevice);

    returnValue.logicalDeviceCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr, // reroute needed
        .flags = 0,
        .queueCreateInfoCount = 0,
        .pQueueCreateInfos = nullptr, // reroute needed
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr, // reroute needed
    };

    return returnValue;
}

[[nodiscard]] uint32_t VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice) {
	uint32_t physicalDeviceQueueFamilyCount{};
    VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> physicalDeviceQueueFamilyProperties(physicalDeviceQueueFamilyCount);
    VulkanPFNs::gpVkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &physicalDeviceQueueFamilyCount, physicalDeviceQueueFamilyProperties.data());

    uint32_t graphicsQueueFamilyIndex = UINT32_MAX;
    for (int i = 0; i < physicalDeviceQueueFamilyCount; i++) {
        if (physicalDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsQueueFamilyIndex = i;
        }
    }

    if (graphicsQueueFamilyIndex == UINT32_MAX) {
        throw std::runtime_error("Did not find a graphics queue for physical device");
    } else {
        return graphicsQueueFamilyIndex;
    }
}