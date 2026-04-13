#pragma once

#include <vulkan/vulkan.h>
#include "LogicalDevice.h"

namespace Backend {
	namespace PhysicalDevice {
		inline VkPhysicalDevice gpPhysicalDevice{};
		inline std::vector<VkPhysicalDevice> gSystemPhysicalDevices{};
		inline std::vector<VkPhysicalDeviceProperties> gSystemPhysicalDeviceProperties{};

		inline std::array<uint32_t, LogicalDevice::gQUEUE_FAMILY_COUNT> gQueueFamilyIndices{};

		inline VkPhysicalDeviceLimits gLimits{};

		void init();
	
		void enumerateSystemPhysicalDevices(std::vector<VkPhysicalDeviceProperties>&, std::vector<VkPhysicalDevice>&);
		void selectPhysicalDevice();

		bool physicalDeviceGood(VkPhysicalDeviceProperties const&, VkPhysicalDevice&);
		bool apiVersionCheck(VkPhysicalDeviceProperties const&);
		bool extensionsCheck(VkPhysicalDevice&);
		bool featuresCheck(VkPhysicalDevice&);
		bool queuesCheck(VkPhysicalDevice&);
	}
}