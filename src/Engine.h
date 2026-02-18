#pragma once

#include "GlobalState.h"
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

	[[nodiscard]] std::vector<VkImage> fGetSwapchainImages();
	[[nodiscard]] VkImageView fGetSwapchainImageView(uint32_t const& IMAGE_INDEX);
	void fRecordDrawCommands(VkCommandBuffer& commandBuffer, uint32_t const& IMAGE_INDEX);
	const VkResult fAcquireNextSwapchainImageIndex(ImageKillhouse& killhouse, uint32_t& nextImageIndex);
	void fSubmitDrawCommands(VkQueue& queue, ImageHitman& hitman);
	const VkResult fQueueImageForPresentation(VkQueue& queue, uint32_t const& SWAPCHAIN_IMAGE_INDEX, ImageHitman& hitman);
	const bool fRecreateSwapchainIfNecessary(VkResult const& RESULT);
	void fInitializegCurrentTransformation();
	[[nodiscard]] const float fGetTimeSinceFirstCall();
	void fReactToInput();
	void fWriteToUniformBuffer();
	void fRunThroughNextSwapchainImage(ImageKillhouse& killhouse);
	void fRenderLoop(ImageKillhouse& killhouse);
}
