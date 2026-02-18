#include "Devices.hpp"
#include <iostream>
#include <array>

namespace Backend {
	Devices::Devices() :
		mpBackend{},
		mpPhysicalDevice{},
		mpLogicalDevice{},
		mParameters{},
		mGRAPHICS_QUEUE_FAMILY_INDEX{ UINT32_MAX } {}

	Devices::Devices(Instance* givenBackend, DevicesConstructParameters const& GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS) :
		mpBackend{ givenBackend },
		mpPhysicalDevice{ GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS.mSelectedPhysicalDevice },
		mpLogicalDevice{},
		mParameters{ GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS },
		mGRAPHICS_QUEUE_FAMILY_INDEX{ GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS.mGraphicsQueueFamilyIndex } {

		// reroute pointers
		mParameters.mDeviceEnabledDynamicRenderingFeatures.pNext = &mParameters.mDeviceEnabledExtendedDynamicStateFeatures;
		mParameters.mDeviceEnabledSyncFeatures.pNext = &mParameters.mDeviceEnabledDynamicRenderingFeatures;
		mParameters.mEnabledDeviceFeatures.pNext = &mParameters.mDeviceEnabledSyncFeatures;

		mParameters.mLogicalDeviceCreateInfo.pNext = &mParameters.mEnabledDeviceFeatures;

		mParameters.mDeviceQueueFamilyCreateInfos[0].pQueuePriorities = mParameters.mDeviceQueueFamilyQueuePriorities[0].data();

		mParameters.mLogicalDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(mParameters.mDeviceQueueFamilyCreateInfos.size());
		mParameters.mLogicalDeviceCreateInfo.pQueueCreateInfos = mParameters.mDeviceQueueFamilyCreateInfos.data();
		mParameters.mLogicalDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(mParameters.mEnabledDeviceExtensions.size());
		mParameters.mLogicalDeviceCreateInfo.ppEnabledExtensionNames = mParameters.mEnabledDeviceExtensions.data();

		// construct the logical device and queues
		CHECK_VK_SUCCESS(
			vkCreateDevice(mpPhysicalDevice, &mParameters.mLogicalDeviceCreateInfo, nullptr, &mpLogicalDevice),
			"Failed to create logical device"
		)
		mGraphicsFamilypQueues.resize(mParameters.mLogicalDeviceCreateInfo.pQueueCreateInfos[0].queueCount, VK_NULL_HANDLE);
		for(int i = 0; i < mGraphicsFamilypQueues.size(); i++) {
			vkGetDeviceQueue(mpLogicalDevice, mParameters.mLogicalDeviceCreateInfo.pQueueCreateInfos[0].queueFamilyIndex, i, &mGraphicsFamilypQueues[i]);
		}
	}

	Devices::~Devices() {
		vkDestroyDevice(mpLogicalDevice, nullptr);
	}

	[[nodiscard]] Devices::DevicesConstructParameters Devices::sGetConstructParameters(VkInstance instance) {
		DevicesConstructParameters returnValue{};

		auto setupEnabledFeatures = [&returnValue]() -> void {
			returnValue.mDeviceEnabledExtendedDynamicStateFeatures = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
				.extendedDynamicState2 = true
			};
			returnValue.mDeviceEnabledDynamicRenderingFeatures = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
				.pNext = nullptr, // reroute needed
				.dynamicRendering = true
			};
			returnValue.mDeviceEnabledSyncFeatures = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
				.pNext = nullptr, // reroute needed
				.synchronization2 = true
			};
			returnValue.mEnabledDeviceFeatures = {
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
				.pNext = nullptr, // reroute needed
				.features = {
					.samplerAnisotropy = true,
					.textureCompressionBC = true
				}
			};
		};
		setupEnabledFeatures();

		auto setupEnabledExtensions = [&returnValue]() -> void {
			returnValue.mEnabledDeviceExtensions = {
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
			returnValue.mDeviceQueueFamilyQueuePriorities = {
				{0.5f}
			};

			returnValue.mDeviceQueueFamilyCreateInfos = {
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
				vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr),
				"Failed to enumerate physical devices on your instance"
			);
			std::vector<VkPhysicalDevice> systemPhysicalDevices(physicalDeviceCount);
			CHECK_VK_SUCCESS(
				vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, systemPhysicalDevices.data()),
				"Failed to enumerate physical devices on your instance"
			);

			// weed out the downs
			for (int i = 0; i < systemPhysicalDevices.size(); i++) {
				VkPhysicalDeviceProperties physicalDeviceProperties{};
				vkGetPhysicalDeviceProperties(systemPhysicalDevices[i], &physicalDeviceProperties);

				// api version check
				if (!(physicalDeviceProperties.apiVersion >= VK_API_VERSION_1_3)) {
					systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
					std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for too low API version of below " << UINT32_TO_VK_API_VERSION_CSTR(VK_API_VERSION_1_3) << "\n";
					continue;
				}

				// has graphics queue family with enough queues check
				uint32_t physicalDeviceQueueFamilyCount{};
				vkGetPhysicalDeviceQueueFamilyProperties(systemPhysicalDevices[i], &physicalDeviceQueueFamilyCount, nullptr);
				std::vector<VkQueueFamilyProperties> physicalDeviceQueueFamilyProperties(physicalDeviceQueueFamilyCount);
				vkGetPhysicalDeviceQueueFamilyProperties(systemPhysicalDevices[i], &physicalDeviceQueueFamilyCount, physicalDeviceQueueFamilyProperties.data());

				bool foundGraphicsQueueFamilyWithEnoughQueues = false;
				for (int i = 0; i < physicalDeviceQueueFamilyCount; i++) {
					if ((physicalDeviceQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
						(physicalDeviceQueueFamilyProperties[i].queueCount >= returnValue.mDeviceQueueFamilyCreateInfos[0].queueCount)) {
						foundGraphicsQueueFamilyWithEnoughQueues = true;
						returnValue.mGraphicsQueueFamilyIndex = i;
					}
				}

				if (!foundGraphicsQueueFamilyWithEnoughQueues) {
					systemPhysicalDevices.erase(systemPhysicalDevices.begin() + i);
					std::cout << "Weeded out " << physicalDeviceProperties.deviceName << " for not having a graphics queue family with eough queues" << "\n";
					continue;
				}

				// extensions check
				uint32_t physicalDeviceExtensionsCount{};
				vkEnumerateDeviceExtensionProperties(systemPhysicalDevices[i], nullptr, &physicalDeviceExtensionsCount, nullptr);
				std::vector<VkExtensionProperties> physicalDeviceExtensions(physicalDeviceExtensionsCount);
				vkEnumerateDeviceExtensionProperties(systemPhysicalDevices[i], nullptr, &physicalDeviceExtensionsCount, physicalDeviceExtensions.data());

				std::vector<std::string> physicalDeviceExtensionNames{};
				for (VkExtensionProperties const& physicalDeviceExtension : physicalDeviceExtensions) {
					physicalDeviceExtensionNames.push_back(physicalDeviceExtension.extensionName);
				}
				std::vector<std::string> logicalDeviceExtensionNames{};
				for (int i = 0; i < returnValue.mEnabledDeviceExtensions.size(); i++) {
					logicalDeviceExtensionNames.push_back(returnValue.mEnabledDeviceExtensions[i]);
				}

				if (!Common::fContainsAll(physicalDeviceExtensionNames, logicalDeviceExtensionNames)) {
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
				vkGetPhysicalDeviceFeatures2(systemPhysicalDevices[i], &physicalDeviceFeaturesStatus);

				if (!(physicalDeviceFeaturesStatus.features.samplerAnisotropy && physicalDeviceFeaturesStatus.features.textureCompressionBC && physicalDeviceSyncFeaturesStatus.synchronization2 && physicalDeviceDynamicRenderingFeaturesStatus.dynamicRendering && physicalDeviceExtendedDynamicStateFeaturesStatus.extendedDynamicState2)) {
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
				vkGetPhysicalDeviceProperties(systemPhysicalDevices[i], &physicalDeviceProperties);

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
			vkGetPhysicalDeviceProperties(selectedPhysicalDevice, &selectedPhysicalDeviceProperties);
			std::cout << "SELECTED PHYSICAL DEVICE: " << selectedPhysicalDeviceProperties.deviceName << "\n";
			std::cout << "DISCRETE GPU? " << ((niceToHavesBySystemPhysicalDevice[selectedPhysicalDeviceIndex][0]) ? "Yes\n" : "No\n");

			returnValue.mSelectedPhysicalDevice = systemPhysicalDevices[selectedPhysicalDeviceIndex];
		};
		selectPhysicalDevice(instance);
    
		returnValue.mDeviceQueueFamilyCreateInfos[0].queueFamilyIndex = returnValue.mGraphicsQueueFamilyIndex;

		returnValue.mLogicalDeviceCreateInfo = {
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
}
