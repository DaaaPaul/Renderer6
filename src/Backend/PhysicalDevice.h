#pragma once

#include <vulkan/vulkan_core.h>
#include <vector>
#include <array>
#include <cstdint>
#include "Backend/LogicalDevice.h"

namespace PhysicalDevice {
	inline VkPhysicalDevice g_physical_device{};

	inline std::array<uint32_t, LogicalDevice::g_QUEUE_FAMILY_COUNT> g_queue_family_indices{};

	inline VkPhysicalDeviceLimits g_limits{};

	void init();
	
	VkPhysicalDevice select_physical_device(const std::vector<VkPhysicalDevice>& system_physical_devices, const std::vector<VkPhysicalDeviceProperties>& system_physical_device_properties);
	std::vector<VkPhysicalDevice> get_physical_devices();
	std::vector<VkPhysicalDeviceProperties> get_physical_device_properties(const std::vector<VkPhysicalDevice>& physical_devices);
	bool physical_device_good(VkPhysicalDeviceProperties physical_device_properties, VkPhysicalDevice physical_device);
	bool check_api_version(VkPhysicalDeviceProperties physical_device_properties);
	bool check_extensions(VkPhysicalDevice physical_device);
	bool check_features(VkPhysicalDevice physical_device);
	bool check_queues(VkPhysicalDevice physical_device);

	uint32_t& get_queue_family_index(VkQueueFlags queue_family_capabilities);
}
