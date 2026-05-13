#pragma once

#include <array>
#include <vector>
#include "FeatureChain.hpp"

namespace LogicalDevice {
	inline VkLogicalDevice g_device{};
	inline std::vector<VkQueue> gQueues{};

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
	VkPhysicalDeviceHostImageCopyFeatures> gFeatures(
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
	inline constexpr uint32_t gQUEUE_FAMILY_COUNT = 1;
	inline constexpr std::array<VkQueueFlags, gQUEUE_FAMILY_COUNT> gQUEUE_FAMILY_CAPABILITIES{
		VK_QUEUE_GRAPHICS_BIT
	};
	inline constexpr std::array<uint32_t, gQUEUE_FAMILY_COUNT> gQUEUES_PER_QUEUE_FAMILY{
		1
	};
	inline const std::array<std::vector<float>, gQUEUE_FAMILY_COUNT> gQUEUE_PRIORITIES{
		std::vector<float>{0.5f}
	};

	void init();
	void destroy();

	void createLogicalDevice();
	void createQueues();
	void destroyLogicalDevice();

	uint32_t getQueueIndex(uint32_t const& QUEUE_FAMILY, uint32_t const& QUEUE_IN_QUEUE_FAMILY);
}

using LogicalDevice::g_device;