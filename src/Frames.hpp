#pragma once

#include <vector>
#include <vulkan/vulkan.h>

namespace Engine {
	struct Frames {
		struct Frame {
			VkCommandPool pPoolUsed{};

			VkFence pOneAtATime{};
			VkSemaphore pTimeline{};
			uint64_t timelineVal{};
			VkCommandBuffer pDrawCommands{};
			VkCommandBuffer pComputeCommands{};

			explicit Frame(VkCommandPool pPool);
			~Frame();
		};

		VkCommandPool pCommandPool{};
		std::vector<Frame> frames{};

		void recreateFrames();

		explicit Frames(uint16_t const& FRAME_COUNT, uint32_t const& GRAPHICS_QF_INDEX);
		~Frames();
	};
}