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
		VkFence mpOneAtATime{};
		VkSemaphore mpRenderReady{};
		VkSemaphore mpRenderFinished{};
		VkCommandPool mpCommandPool{};
		VkCommandBuffer mDrawCommands{};

		explicit ImageHitman(VkCommandPool pool);
		~ImageHitman();
	};

	struct ImageKillhouse {
		VkCommandPool mpCommandPoolUsed{};
		std::vector<ImageHitman> mHitmen{};

		void recreateHitmen();

		explicit ImageKillhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX);
		~ImageKillhouse();
	};

	inline uint32_t gHitmanIndex{};
	inline Vertex::Transforms gCurrentTransformation{};

	[[nodiscard]] std::vector<VkImage> getSwapchainImages();
	[[nodiscard]] VkImageView getSwapchainImageView(uint32_t const& IMAGE_INDEX, VkFormat const& FORMAT, VkImageSubresourceRange const& SUBRESOURCE_RANGE);
	void recordDrawCommands(VkCommandBuffer& commandBuffer, uint32_t const& IMAGE_INDEX);
	VkResult acquireNextSwapchainImageIndex(ImageKillhouse& killhouse, uint32_t& nextImageIndex);
	void submitDrawCommands(VkQueue& queue, ImageHitman& hitman);
	VkResult queueImageForPresentation(VkQueue& queue, uint32_t const& SWAPCHAIN_IMAGE_INDEX, ImageHitman& hitman);
	bool recreateSwapchainIfNecessary(VkResult const& RESULT);
	void initializeCurrentTransformation();
	[[nodiscard]] float getTimeSinceFirstCall();
	void reactToInput();
	void writeToUniformBuffer();
	void runThroughNextSwapchainImage(ImageKillhouse& killhouse);
	void renderLoop(ImageKillhouse& killhouse);
}
