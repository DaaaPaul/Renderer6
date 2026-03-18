#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace LogicalDevice {
	void init() {
		createLogicalDevice();
		createQueues();
	}

	void deInit() {
		destroyLogicalDevice();
		destroyQueues();
	}

	void createLogicalDevice() {
		std::vector<VkDeviceQueueCreateInfo> queuesCreate(gQUEUE_FAMILY_COUNT, {});
		for(int i = 0; i < gQUEUE_FAMILY_COUNT; i++) {
			queuesCreate[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			queuesCreate[i].queueFamilyIndex = gQueueFamilyIndices[i];
			queuesCreate[i].queueCount = gQUEUES_PER_QUEUE_FAMILY[i];
			queuesCreate[i].pQueuePriorities = gQUEUE_PRIORITIES[i].data();
		}

		VkDeviceCreateInfo logicalDeviceCreate{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &gFeatures.val,
			.queueCreateInfoCount = gQUEUE_FAMILY_COUNT,
			.pQueueCreateInfos = queuesCreate.data(),
			.enabledExtensionCount = UINT32(gExtensions.size()),
			.ppEnabledExtensionNames = gExtensions.data(),
		};

		CHECK_VK_SUCCESS(vkCreateDevice(PhysicalDevice::gpPhysicalDevice, &logicalDeviceCreate, nullptr, &gpDevice), "Failed to create logical device")
	}

	void createQueues() {
		for(int i = 0; i < gQUEUE_FAMILY_COUNT; i++) {
			for(int j = 0; j < gQUEUE_PRIORITIES[i].size(); j++) {
				vkGetDeviceQueue(gpDevice, gQueueFamilyIndices[i], j, gQueues[])
			}
		}
	}

	void destroyLogicalDevice() noexcept {

	}

	void destroyQueues() noexcept {
	
	}
}