#include <vulkan/vulkan_core.h>
#include <vector>
#include <stdexcept>
#include <cstdint>
#include <algorithm>
#include <string>
#include "Backend/PhysicalDevice.h"
#include "Backend/LogicalDevice.h"
#include "Backend/Instance.h"
#include "Utility/Vulkan.h"
#include "Utility/Utility.h"

namespace PhysicalDevice {
	void init() {
		std::vector<VkPhysicalDevice> physical_devices(get_physical_devices());
		g_physical_device = select_physical_device(physical_devices, get_physical_device_properties(physical_devices));

		if(!g_physical_device) {
			THROW_RUNTIME("Failed to select a GPU on your system");
		} else {
			VkPhysicalDeviceProperties selected_properties{};
			vkGetPhysicalDeviceProperties(g_physical_device, &selected_properties);
			g_limits = selected_properties.limits;
		}
	}
	
	VkPhysicalDevice select_physical_device(const std::vector<VkPhysicalDevice>& system_physical_devices, const std::vector<VkPhysicalDeviceProperties>& system_physical_device_properties) {
		assert(system_physical_devices.size() == system_physical_device_properties.size());
		VkPhysicalDevice selected = nullptr;

		for(int i = 0; i < system_physical_devices.size() && !selected; ++i) {
			if(physical_device_good(system_physical_device_properties[i], system_physical_devices[i])) {
				selected = system_physical_devices[i];
			}
		}
		
		return selected;
	}

	std::vector<VkPhysicalDevice> get_physical_devices() {
		uint32_t physical_device_count{};
		VK_CHECK(vkEnumeratePhysicalDevices(Instance::g_instance, &physical_device_count, nullptr), "get_physical_devices: failed to enumerate physical devices");
		std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
		VK_CHECK(vkEnumeratePhysicalDevices(Instance::g_instance, &physical_device_count, physical_devices.data()), "get_physical_devices: failed to enumerate physical devices");
			
		return physical_devices;
	}

	std::vector<VkPhysicalDeviceProperties> get_physical_device_properties(const std::vector<VkPhysicalDevice>& physical_devices) {
		std::vector<VkPhysicalDeviceProperties> physical_device_properties(physical_devices.size());

		for(int i = 0; i < physical_devices.size(); ++i) {
			vkGetPhysicalDeviceProperties(physical_devices[i], &physical_device_properties[i]);
		}

		return physical_device_properties;
	}

	bool physical_device_good(VkPhysicalDeviceProperties properties, VkPhysicalDevice physical_device) {
		return check_api_version(properties) && check_extensions(physical_device) && check_features(physical_device) && check_queues(physical_device);
	}

	bool check_api_version(VkPhysicalDeviceProperties properties) {
		return properties.apiVersion >= VK_API_VERSION_1_3;
	}

	bool check_extensions(VkPhysicalDevice physical_device) {
		uint32_t extensions_count{};
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, nullptr);
		std::vector<VkExtensionProperties> extensions(extensions_count);
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extensions_count, extensions.data());

		std::vector<std::string> extension_strings{};
		for(const VkExtensionProperties& extension : extensions) {
			extension_strings.emplace_back(extension.extensionName);
		}

		return Utility::contains_all(extension_strings, Utility::to_string(LogicalDevice::g_extensions));
	}

	bool check_features(VkPhysicalDevice physical_device) {
		auto feature_structure(LogicalDevice::g_features);
		vkGetPhysicalDeviceFeatures2(physical_device, &feature_structure.feature);

		return feature_structure.has_all(LogicalDevice::g_features);
	}

	bool check_queues(VkPhysicalDevice physical_device) {
		bool has_queues = true;

		uint32_t queue_family_count = UINT32_MAX;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

		g_queue_family_indices.fill(UINT32_MAX);
		for(int i = 0; i < LogicalDevice::g_QUEUE_FAMILY_COUNT && has_queues; ++i) {
			for(int j = 0; j < queue_family_count && g_queue_family_indices[i] == UINT32_MAX; ++j) {
				if ((queue_families[j].queueFlags & LogicalDevice::g_QUEUE_FAMILY_CAPABILITIES[i]) && 
					(queue_families[j].queueCount >= LogicalDevice::g_QUEUE_FAMILY_QUEUES[i])) {
					g_queue_family_indices[i] = j;
				}
			}

			if(g_queue_family_indices[i] == UINT32_MAX) {
				has_queues = false;
			}
		}

		return has_queues;
	}

	uint32_t get_queue_family_index(VkQueueFlags queue_family_capabilities) {
		size_t queue_family_array_index = UINT64_MAX;
		
		for(int i = 0; i < LogicalDevice::g_QUEUE_FAMILY_COUNT && queue_family_array_index == UINT64_MAX; ++i) {
			if ((LogicalDevice::g_QUEUE_FAMILY_CAPABILITIES[i] & queue_family_capabilities) == queue_family_capabilities) {
				queue_family_array_index = i;
			}
		}

		return g_queue_family_indices[queue_family_array_index];
	}
}
