#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace LogicalDevice {
	void init() {
		createLogicalDevice();
		createQueues();
	}

	void destroy() {
		destroyLogicalDevice();
	}

	void createLogicalDevice() {
		std::vector<VkDeviceQueueCreateInfo> queuesCreate(gQUEUE_FAMILY_COUNT, {});
		for(int i = 0; i < gQUEUE_FAMILY_COUNT; ++i) {
			queuesCreate[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			queuesCreate[i].queueFamilyIndex = PhysicalDevice::g_queue_family_indices[i];
			queuesCreate[i].queueCount = gQUEUES_PER_QUEUE_FAMILY[i];
			queuesCreate[i].pQueuePriorities = gQUEUE_PRIORITIES[i].data();
		}

		VkDeviceCreateInfo logicalDeviceCreate{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = &gFeatures.feature,
			.queueCreateInfoCount = gQUEUE_FAMILY_COUNT,
			.pQueueCreateInfos = queuesCreate.data(),
			.enabledExtensionCount = UINT32(gExtensions.size()),
			.ppEnabledExtensionNames = gExtensions.data(),
		};

		VK_CHECK(vkCreateDevice(PhysicalDevice::gPhysicalDevice, &logicalDeviceCreate, nullptr, &g_device), "Failed to create logical device")
	}

	void createQueues() {
		gQueues.resize(getQueueIndex(gQUEUE_FAMILY_COUNT - 1, gQUEUES_PER_QUEUE_FAMILY[gQUEUE_FAMILY_COUNT - 1]) + 1, VK_NULL_HANDLE);

		for(int i = 0; i < gQUEUE_FAMILY_COUNT; ++i) {
			for(int j = 0; j < gQUEUES_PER_QUEUE_FAMILY[i]; j++) {
				vkGetDeviceQueue(g_device, PhysicalDevice::g_queue_family_indices[i], j, &gQueues[getQueueIndex(i, j)]);
			}
		}
	}

	void destroyLogicalDevice() {
		vkDestroyDevice(g_device, nullptr);
	}

	uint32_t getQueueIndex(uint32_t const& QUEUE_FAMILY, uint32_t const& QUEUE_IN_QUEUE_FAMILY) {
		uint32_t queueConsumer = 0;

		for(int i = 0; i < QUEUE_FAMILY; ++i) {
			queueConsumer += gQUEUES_PER_QUEUE_FAMILY[i];
		}

		return queueConsumer + QUEUE_IN_QUEUE_FAMILY;
	}
}
