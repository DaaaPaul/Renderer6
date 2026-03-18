#include "LogicalDevice.h"
#include "PhysicalDevice.h"

namespace Backend {
	namespace LogicalDevice {
		void init() {
			createLogicalDevice();
			createQueues();
		}

		void deInit() {
			destroyLogicalDevice();
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
			auto getQueueIndex = [](uint32_t const& QF, uint32_t const& Q_IN_QF) -> uint32_t {
				uint32_t queueConsumer = 0;
				for(int i = 0; i < QF; i++) {
					queueConsumer += gQUEUES_PER_QUEUE_FAMILY[i];
				}
				return queueConsumer + Q_IN_QF;
			};

			for(int i = 0; i < gQUEUE_FAMILY_COUNT; i++) {
				for(int j = 0; j < gQUEUES_PER_QUEUE_FAMILY[i]; j++) {
					vkGetDeviceQueue(gpDevice, gQueueFamilyIndices[i], j, &gQueues[getQueueIndex(i, j)]);
				}
			}
		}

		void destroyLogicalDevice() noexcept {
			vkDestroyDevice(gpDevice, nullptr);
		}
	}
}