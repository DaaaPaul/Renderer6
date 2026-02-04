#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "VulkanPFNs.h"
#include "Common.h"

namespace RenderEngine {
	inline GLFWwindow* gpGlfwWindowUsed{};
	inline VkDevice gpLogicalDeviceUsed{};

	struct ImageKiller {
		struct ImageHitmanEquipment {
			VkSemaphore mRenderReady{};
			VkSemaphore mRenderFinished{};
			VkCommandPool mCommandPool{};
			VkCommandBuffer mDrawCommands{};

			explicit ImageHitmanEquipment(VkCommandPool pool);
			~ImageHitmanEquipment();
		};

		VkCommandPool mpCommandPoolUsed{};
		std::vector<ImageHitmanEquipment> mHitmen{};

		explicit ImageKiller(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX);
		~ImageKiller();
	};

	void fRenderLoop();
}
