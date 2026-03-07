#include "Devices.hpp"
#include <iostream>
#include <array>

namespace Backend {
	Devices::Devices(Instance* pGivenInstance, CreateInfo&& givenCreateInfo) :
		pInstance{ pGivenInstance },
		pPhysicalDevice{},
		pLogicalDevice{},
		CREATE_INFO{ std::move(givenCreateInfo) },
		GRAPHICS_QF_INDEX{ CREATE_INFO.queueFamilyInfos[0].queueFamilyIndex } {

		pPhysicalDevice = CREATE_INFO.pPhysicalDevice;

		CHECK_VK_SUCCESS(
			vkCreateDevice(pPhysicalDevice, &CREATE_INFO.logicalDeviceInfo, nullptr, &pLogicalDevice),
			"Failed to create logical device"
		)
		graphicsQueues.resize(CREATE_INFO.logicalDeviceInfo.pQueueCreateInfos[0].queueCount, VK_NULL_HANDLE);
		for(int i = 0; i < graphicsQueues.size(); i++) {
			vkGetDeviceQueue(pLogicalDevice, CREATE_INFO.logicalDeviceInfo.pQueueCreateInfos[0].queueFamilyIndex, i, &graphicsQueues[i]);
		}
	}

	Devices::~Devices() {
		vkDestroyDevice(pLogicalDevice, nullptr);
	}
}
