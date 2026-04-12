#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "Swapchain.h"

namespace Engine {
	namespace FrameData {
		struct FrameData {
			VkFence oneAtATime{};
			VkSemaphore timeline{};
			uint64_t timelineVal{};
			VkCommandBuffer modelCmds{};
			VkCommandBuffer particleCmds{};
			VkCommandBuffer computeCmds{};
		};

		inline VkCommandPool gCmdPool{};
		inline std::vector<FrameData> gFrameData{};
		inline uint32_t gFrameIndex = 0;
		inline constexpr uint32_t gFRAMES_IN_FLIGHT = Swapchain::gIMAGE_COUNT;

		void init();
		void deInit() noexcept;
		void recreate();

		void createCmdPool();
		void createFrameData();

		void clearFrameData() noexcept;
	}
}