#include "Devices.hpp"
#include <iostream>
#include <array>

namespace Backend {
	Devices::Devices(Instance* givenInstance, CreateInfo&& givenCreateInfo) :
		instance{ givenInstance },
		physicalDevice{},
		logicalDevice{},
		CREATE_INFO{ std::move(givenCreateInfo) },
		GRAPHICS_QF_INDEX{ UINT32_MAX } {

		// hit the physical device
		physicalDevice = CREATE_INFO.physicalDevice;

		// construct the logical device and queues
		CHECK_VK_SUCCESS(
			vkCreateDevice(physicalDevice, &CREATE_INFO.logicalDeviceInfo, nullptr, &logicalDevice),
			"Failed to create logical device"
		)
		graphicsQueues.resize(CREATE_INFO.logicalDeviceInfo.pQueueCreateInfos[0].queueCount, VK_NULL_HANDLE);
		for(int i = 0; i < graphicsQueues.size(); i++) {
			vkGetDeviceQueue(logicalDevice, CREATE_INFO.logicalDeviceInfo.pQueueCreateInfos[0].queueFamilyIndex, i, &graphicsQueues[i]);
		}
	}

	Devices::~Devices() {
		vkDestroyDevice(logicalDevice, nullptr);
	}
}
