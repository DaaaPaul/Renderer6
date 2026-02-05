#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "VulkanSwapchainWrapper.hpp"
#include "VulkanDeviceLocalMemory.hpp"
#include "VulkanPFNs.h"
#include "Common.h"

namespace RenderEngine {
	inline VulkanSwapchainWrapper* gpVulkanSwapchainWrapper{};
	inline VulkanDeviceLocalMemory* gpVulkanDeviceLocalMemory{};

	struct ImageHitmanEquipment {
		VkFence mpOneAtATime{};
		VkSemaphore mpRenderReady{};
		VkSemaphore mpRenderFinished{};
		VkCommandPool mpCommandPool{};
		VkCommandBuffer mDrawCommands{};

		explicit ImageHitmanEquipment(VkCommandPool pool);
		~ImageHitmanEquipment();
	};

	struct ImageKillhouse {
		VkCommandPool mpCommandPoolUsed{};
		std::vector<ImageHitmanEquipment> mHitmen{};

		explicit ImageKillhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX);
		~ImageKillhouse();
	};

	void fRenderLoop(ImageKillhouse& imageKillhouse);
}
