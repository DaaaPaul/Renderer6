#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <array>
#include <cstdint>
#include "LogicalDevice.h"

namespace PhysicalDevice {
	inline VkPhysicalDevice gPhysicalDevice{};
	inline std::vector<VkPhysicalDevice> gSystemPhysicalDevices{};
	inline std::vector<VkPhysicalDeviceProperties> gSystemPhysicalDeviceProperties{};

	inline std::array<uint32_t, LogicalDevice::gQUEUE_FAMILY_COUNT> g_queue_family_indices{};

	inline VkPhysicalDeviceLimits gLimits{};

	void init();
	
	void enumerateSystemPhysicalDevices();
	void selectPhysicalDevice();

	bool physicalDeviceGood(VkPhysicalDeviceProperties const&, VkPhysicalDevice);
	bool apiVersionCheck(VkPhysicalDeviceProperties const&);
	bool extensionsCheck(VkPhysicalDevice);
	bool featuresCheck(VkPhysicalDevice);
	bool queuesCheck(VkPhysicalDevice);
}
