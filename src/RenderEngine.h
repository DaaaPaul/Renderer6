#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "VulkanPFNs.h"
#include "Common.h"

namespace RenderEngine {
	inline GLFWwindow* gGlfwWindowUsed{};
	inline VkDevice gLogicalDeviceUsed{};
	inline VkCommandPool gCommandPoolUsed{};

	struct ImageHitmanEquipment {
		VkSemaphore mRenderReady{};
		VkSemaphore mRenderFinished{};
		VkCommandBuffer mDrawCommands{};

		explicit ImageHitmanEquipment();
		~ImageHitmanEquipment();
	};

	void fRenderLoop();
}
