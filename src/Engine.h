#pragma once

#include "Devices.hpp"
#include "Swapchain.hpp"
#include "HostVisible.hpp"
#include "DeviceLocal.hpp"
#include "GraphicsPipeline.hpp"
#include "Vertex.hpp"
#include "Transforms.hpp"
#include "Common.h"

namespace Engine {
	struct ImageHitman {
		VkFence pOneAtATime{};
		VkSemaphore pRenderReady{};
		VkSemaphore pRenderFinished{};
		VkCommandPool pCommandPool{};
		VkCommandBuffer pDrawCommands{};

		explicit ImageHitman(VkCommandPool pool);
		~ImageHitman();
	};

	struct ImageKillhouse {
		VkCommandPool pCommandPoolUsed{};
		std::vector<ImageHitman> hitmen{};

		void recreateHitmen();

		explicit ImageKillhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX);
		~ImageKillhouse();
	};

	inline uint32_t gHitmanIndex{};
	inline Vertex::Transforms gCurrentTransformation{};

	[[nodiscard]] std::vector<VkImage> getSwapchainImages();
	[[nodiscard]] VkImageView getSwapchainImageView(uint32_t const& IMAGE_INDEX, VkFormat const& FORMAT, VkImageSubresourceRange const& SUBRESOURCE_RANGE);
	void recordDrawCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX);
	VkResult acquireNextSwapchainImageIndex(ImageKillhouse& killhouse, uint32_t& nextImageIndex);
	void submitDrawCommands(VkQueue& pQueue, ImageHitman& hitman);
	VkResult queueImageForPresentation(VkQueue& pQueue, uint32_t const& SWAPCHAIN_IMAGE_INDEX, ImageHitman& hitman);
	bool recreateSwapchainIfNecessary(VkResult const& RESULT);
	void initializeCurrentTransformation();
	[[nodiscard]] float getTimeSinceFirstCall();
	void reactToInput();
	void writeToUniformBuffer();
	void runThroughNextSwapchainImage(ImageKillhouse& killhouse);
	void renderLoop(ImageKillhouse& killhouse);
}
