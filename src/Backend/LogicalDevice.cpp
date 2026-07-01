#include "Backend/LogicalDevice.h"
#include "Backend/PhysicalDevice.h"
#include "Utility/Utility.h"
#include "Utility/Vulkan.h"

namespace LogicalDevice {
	void init() {
		g_device = create_logical_device(PhysicalDevice::g_physical_device,
			PhysicalDevice::g_queue_family_indices,
			g_QUEUE_FAMILY_QUEUES,
			g_QUEUE_PRIORITIES,
			&g_features.feature,
			g_extensions);
		g_queues = create_queues(PhysicalDevice::g_queue_family_indices);
	}

	void destroy() {
		vkDestroyDevice(g_device, nullptr);
	}

	VkDevice create_logical_device(VkPhysicalDevice physical_device,
		const std::array<uint32_t, g_QUEUE_FAMILY_COUNT>& queue_family_indices,
		const std::array<uint32_t, g_QUEUE_FAMILY_COUNT>& queue_family_queues,
		const std::array<std::vector<float>, g_QUEUE_FAMILY_COUNT>& queue_priorities,
		void* p_features,
		const std::vector<const char*>& extensions) {

		VkDevice logical_device{};

		std::vector<VkDeviceQueueCreateInfo> queue_creates(g_QUEUE_FAMILY_COUNT);
		for(int i = 0; i < g_QUEUE_FAMILY_COUNT; ++i) {
			queue_creates[i].sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
			queue_creates[i].queueFamilyIndex = queue_family_indices[i];
			queue_creates[i].queueCount = queue_family_queues[i];
			queue_creates[i].pQueuePriorities = queue_priorities[i].data();
		}

		VkDeviceCreateInfo logical_device_create{
			.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
			.pNext = p_features,
			.queueCreateInfoCount = g_QUEUE_FAMILY_COUNT,
			.pQueueCreateInfos = queue_creates.data(),
			.enabledExtensionCount = UINT32(extensions.size()),
			.ppEnabledExtensionNames = extensions.data(),
		};

		VK_CHECK(vkCreateDevice(physical_device, &logical_device_create, nullptr, &logical_device), "create_logical_device: failed")

		return logical_device;
	}

	std::vector<VkQueue> create_queues(const std::array<uint32_t, g_QUEUE_FAMILY_COUNT>& queue_family_indices) {
		std::vector<VkQueue> queues(calculate_queue_index(g_QUEUE_FAMILY_COUNT - 1, g_QUEUE_FAMILY_QUEUES[g_QUEUE_FAMILY_COUNT - 1]) + 1);

		for(int i = 0; i < g_QUEUE_FAMILY_COUNT; ++i) {
			for(int j = 0; j < g_QUEUE_FAMILY_QUEUES[i]; ++j) {
				vkGetDeviceQueue(g_device, queue_family_indices[i], j, &queues[calculate_queue_index(i, j)]);
			}
		}

		return queues;
	}

	VkQueue get_queue(VkQueueFlags queue_family_capabilities, uint32_t queue_index) {
		size_t queue_family_array_index = UINT64_MAX;
		for(int i = 0; i < g_QUEUE_FAMILY_COUNT && queue_family_array_index == UINT64_MAX; ++i) {
			if ((g_QUEUE_FAMILY_CAPABILITIES[i] & queue_family_capabilities) == queue_family_capabilities) {
				queue_family_array_index = i;
			}
		}

		size_t linear_queue_index = calculate_queue_index(queue_family_array_index, queue_index);
		if(linear_queue_index >= g_queues.size()) {
			THROW_RUNTIME("Attempted to retrieve queue that doesn't exist");
		} else {
			return g_queues[linear_queue_index];
		}
	}
}
