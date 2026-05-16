#pragma once

#include <array>
#include <vector>
#include "FeatureChain.hpp"

namespace LogicalDevice {
	inline VkLogicalDevice g_device{};
	inline std::vector<VkQueue> g_queues{};
	inline constexpr uint32_t g_QUEUE_FAMILY_COUNT = 1;
	inline constexpr std::array<VkQueueFlags, g_QUEUE_FAMILY_COUNT> g_QUEUE_FAMILY_CAPABILITIES{
		VK_QUEUE_GRAPHICS_BIT
	};
	inline constexpr std::array<uint32_t, g_QUEUE_FAMILY_COUNT> g_QUEUE_FAMILY_QUEUES{
		1
	};
	inline const std::array<std::vector<float>, g_QUEUE_FAMILY_COUNT> g_QUEUE_PRIORITIES{
		std::vector<float>{0.5f}
	};

	inline std::vector<const char*> g_extensions{
		"VK_KHR_swapchain",
		"VK_KHR_synchronization2",
		"VK_KHR_spirv_1_4",
		"VK_EXT_host_image_copy",
		#ifdef __APPLE__
		"VK_KHR_portability_subset"
		#endif
	};
	inline FeatureChain<VkPhysicalDeviceFeatures2, 
	VkPhysicalDeviceBufferDeviceAddressFeatures, 
	VkPhysicalDeviceTimelineSemaphoreFeatures, 
	VkPhysicalDeviceSynchronization2Features, 
	VkPhysicalDeviceDynamicRenderingFeatures, 
	VkPhysicalDeviceExtendedDynamicState2FeaturesEXT,
	VkPhysicalDeviceHostImageCopyFeatures> g_features(
		VkPhysicalDeviceFeatures2{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
			.features = {
				.samplerAnisotropy = VK_TRUE,
				.textureCompressionBC = VK_TRUE
			}
		},
		VkPhysicalDeviceBufferDeviceAddressFeatures{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
			.bufferDeviceAddress = VK_TRUE
		},
		VkPhysicalDeviceTimelineSemaphoreFeatures{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
			.timelineSemaphore = VK_TRUE
		},
		VkPhysicalDeviceSynchronization2Features{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
			.synchronization2 = VK_TRUE
		},
		VkPhysicalDeviceDynamicRenderingFeatures{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
			.dynamicRendering = VK_TRUE
		},
		VkPhysicalDeviceExtendedDynamicState2FeaturesEXT{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
			.extendedDynamicState2 = VK_TRUE
		},
		VkPhysicalDeviceHostImageCopyFeaturesEXT{
			.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_IMAGE_COPY_FEATURES_EXT,
			.hostImageCopy = VK_TRUE
		}
	);

	void init();
	void destroy();

	VkLogicalDevice create_logical_device(VkPhysicalDevice physical_device,
		const std::array<uint32_t, LogicalDevice::g_QUEUE_FAMILY_COUNT>& queue_family_indices,
		const std::array<uint32_t, g_QUEUE_FAMILY_COUNT>& queue_counts,
		const std::array<std::vector<float>, g_QUEUE_FAMILY_COUNT>& queue_priorities,
		void* p_features,
		const std::vector<const char*>& extensions);
	std::vector<VkQueue> create_queues(const std::array<uint32_t, g_QUEUE_FAMILY_COUNT>& queue_family_indices);
	inline uint32_t calculate_queue_index(uint32_t queue_family_array_index, uint32_t queue_index) {
		uint32_t tally = 0;

		for(int i = 0; i < queue_family_array_index; ++i) {
			tally += g_QUEUE_FAMILY_QUEUES[i];
		}

		return tally + queue_index;
	}

	VkQueue get_queue(VkQueueFlags queue_family_capabilities, uint32_t queue_index = 0);
}

using LogicalDevice::g_device;