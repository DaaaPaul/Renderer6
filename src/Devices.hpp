#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "Instance.hpp"

namespace Backend {
	struct Devices {
		struct DevicesConstructParameters {
			VkPhysicalDevice mSelectedPhysicalDevice{};
			VkDeviceCreateInfo mLogicalDeviceCreateInfo{};
			std::vector<VkDeviceQueueCreateInfo> mDeviceQueueFamilyCreateInfos{};
			std::vector<std::vector<float>> mDeviceQueueFamilyQueuePriorities{};
			uint32_t mGraphicsQueueFamilyIndex{ UINT32_MAX };
			std::vector<const char*> mEnabledDeviceExtensions{};
			VkPhysicalDeviceExtendedDynamicState2FeaturesEXT mDeviceEnabledExtendedDynamicStateFeatures{};
			VkPhysicalDeviceDynamicRenderingFeatures mDeviceEnabledDynamicRenderingFeatures{};
			VkPhysicalDeviceSynchronization2Features mDeviceEnabledSyncFeatures{};
			VkPhysicalDeviceFeatures2 mEnabledDeviceFeatures{};
		};

		Instance* mpBackend{};
		VkPhysicalDevice mpPhysicalDevice{};
		VkDevice mpLogicalDevice{};
		std::vector<VkQueue> mGraphicsFamilypQueues{};
		DevicesConstructParameters mParameters{};
		const uint32_t mGRAPHICS_QUEUE_FAMILY_INDEX{ UINT32_MAX };

		[[nodiscard]] static DevicesConstructParameters sGetConstructParameters(VkInstance instance);

		Devices();
		explicit Devices(Instance* givenBackend, DevicesConstructParameters const& GIVEN_VULKAN_DEVICES_WRAPPER_CONSTRUCT_PARAMETERS);
		~Devices();

		DELETE_COPY_CONSTRUCTORS(Devices)
		DELETE_MOVE_CONSTRUCTORS(Devices)
	};
}