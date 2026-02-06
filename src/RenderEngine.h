#pragma once

#include "VulkanDevicesWrapper.hpp"
#include "VulkanSwapchainWrapper.hpp"
#include "VulkanDeviceLocalMemory.hpp"
#include "VulkanGraphicsPipelineWrapper.hpp"
#include "VulkanPFNs.h"
#include "Common.h"

namespace RenderEngine {
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

		explicit ImageKillhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX);
		~ImageKillhouse();
	};

	inline VulkanGraphicsPipelineWrapper* gpVulkanGraphicsPipelineWrapper{};
	inline VulkanSwapchainWrapper* gpVulkanSwapchainWrapper{};
	inline VulkanDeviceLocalMemory* gpVulkanDeviceLocalMemory{};

	[[nodiscard]] std::vector<VkImage> fGetSwapchainImages();
	[[nodiscard]] VkImageView fGetImageView(uint32_t const& IMAGE_INDEX);
	void fInsertImageMemoryBarrier2(VkCommandBuffer& commandBuffer, uint32_t const& IMAGE_INDEX, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
		VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
		VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX);
	void fRecordDrawCommands(VkCommandBuffer& commandBuffer, uint32_t const& IMAGE_INDEX);
	void fAcquireNextSwapchainImageIndex(ImageKillhouse& killhouse, uint32_t& nextImageIndex);
	void fSubmitDrawCommands(VkQueue& queue, ImageHitman& hitman);
	void fQueueImageForPresentation(VkQueue& queue, uint32_t const& SWAPCHAIN_IMAGE_INDEX, ImageHitman& hitman);
	void fRunThroughNextSwapchainImage(ImageKillhouse& killhouse);
	void fRenderLoop(ImageKillhouse& killhouse);
}
