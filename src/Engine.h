#pragma once

#include <vulkan/vulkan.h>

namespace Engine {
	struct Hitman {
		VkFence pOneAtATime{};
		VkSemaphore pTimeline{};
		VkCommandPool pCommandPool{};
		VkCommandBuffer pDrawCommands{};
		VkCommandBuffer pComputeCommands{};

		explicit Hitman(VkCommandPool pCommandPool);
		~Hitman();
	};

	struct Killhouse {
		VkCommandPool pCommandPoolUsed{};
		std::vector<Hitman> hitmen{};

		void recreateHitmen();

		Killhouse();
		explicit Killhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX);
		~Killhouse();
	};

	[[nodiscard]] float getDeltaTime() noexcept;
	void recordDrawCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	void recordComputeCommands(VkCommandBuffer& pCommandBuffer);
	void windowResizeRecreate();
	void freshenTransformation();
	void runNextSwapchainImage();
	void renderLoop();
}
