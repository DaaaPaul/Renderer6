#include <iostream>
#include "RenderEngine.h"

namespace RenderEngine {
	ImageHitman::ImageHitman(VkCommandPool pool) :
	mpRenderReady{},
	mpRenderFinished{},
	mpCommandPool{ pool },
	mDrawCommands{} {
		// create fence
		{
			const VkFenceCreateInfo FENCE_INFO{
				.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
				.flags = VK_FENCE_CREATE_SIGNALED_BIT
			};
			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkCreateFence(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, &FENCE_INFO, nullptr, &mpOneAtATime),
				"Fence creation failed"
			)
		}

		// create semaphores
		{
			const VkSemaphoreCreateInfo EMPTY_INFO{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};
			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkCreateSemaphore(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, &EMPTY_INFO, nullptr, &mpRenderReady),
				"Semaphore creation failed"
			)
			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkCreateSemaphore(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, &EMPTY_INFO, nullptr, &mpRenderFinished),
				"Semaphore creation failed"
			)
		}

		// create command buffer
		{
			const VkCommandBufferAllocateInfo DRAW_COMMANDS_INFO{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = mpCommandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};

			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkAllocateCommandBuffers(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, &DRAW_COMMANDS_INFO, &mDrawCommands),
				"Command buffer creation failed"
			)
		}
	}

	ImageHitman::~ImageHitman() {
		VulkanPFNs::gpVkDestroyFence(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, mpOneAtATime, nullptr);
		VulkanPFNs::gpVkDestroySemaphore(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, mpRenderReady, nullptr);
		VulkanPFNs::gpVkDestroySemaphore(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, mpRenderFinished, nullptr);
		VulkanPFNs::gpVkFreeCommandBuffers(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, mpCommandPool, 1, &mDrawCommands);
	}

	ImageKillhouse::ImageKillhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX) : 
	mpCommandPoolUsed{},
	mHitmen{} {
		std::cout << "SET IMAGE KILLER CREATE PARAMETERS:\n";

		const VkCommandPoolCreateInfo POOL_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = GRAPHICS_QF_INDEX
		};
		std::cout << "\tcommand pool flags: " << POOL_INFO.flags << "\n";
		std::cout << "\tcommand pool command buffers submitted to qf: " << POOL_INFO.queueFamilyIndex << "\n";
		std::cout << "\tcreated: " << HITMEN_COUNT << " hitmen" << "\n";

		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkCreateCommandPool(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, &POOL_INFO, nullptr, &mpCommandPoolUsed),
			"Failed to create command pool"
		)

		mHitmen.reserve(HITMEN_COUNT);
		for(int i = 0; i < HITMEN_COUNT; i++) {
			mHitmen.emplace_back(mpCommandPoolUsed);
		}
	}

	ImageKillhouse::~ImageKillhouse() {
		mHitmen.clear();
		VulkanPFNs::gpVkDestroyCommandPool(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, mpCommandPoolUsed, nullptr);
	}

	[[nodiscard]] std::vector<VkImage> fGetSwapchainImages() {
		uint32_t swapchainImageCount{};
		VulkanPFNs::gpVkGetSwapchainImagesKHR(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, gpVulkanSwapchainWrapper->mpSwapchainKHR, &swapchainImageCount, nullptr);
		std::vector<VkImage> swapchainImages(swapchainImageCount);
		VulkanPFNs::gpVkGetSwapchainImagesKHR(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, gpVulkanSwapchainWrapper->mpSwapchainKHR, &swapchainImageCount, swapchainImages.data());

		return swapchainImages;
	}

	void fInsertImageMemoryBarrier2(VkCommandBuffer& commandBuffer, uint32_t const& IMAGE_INDEX, VkImageSubresourceRange const& SUBRESOURCE_RANGE,
		VkPipelineStageFlags2 const& SRC_STAGE, VkAccessFlags2 const& SRC_ACCESS, 
		VkPipelineStageFlags2 const& DST_STAGE, VkAccessFlags2 const& DST_ACCESS, VkImageLayout const& OLD_LAYOUT, VkImageLayout const& NEW_LAYOUT, uint32_t const& GRAPHICS_QF_INDEX) {
		const VkImageMemoryBarrier2 IMAGE_MEMORY_BARRIER2{
			.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
			.srcStageMask = SRC_STAGE,
			.srcAccessMask = SRC_ACCESS,
			.dstStageMask = DST_STAGE,
			.dstAccessMask = DST_ACCESS,
			.oldLayout = OLD_LAYOUT,
			.newLayout = NEW_LAYOUT,
			.srcQueueFamilyIndex = GRAPHICS_QF_INDEX,
			.dstQueueFamilyIndex = GRAPHICS_QF_INDEX,
			.image = fGetSwapchainImages()[IMAGE_INDEX],
			.subresourceRange = SUBRESOURCE_RANGE,
		};

		const VkDependencyInfo PARENT_MEMORY_BARRIER2{
			.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
			.imageMemoryBarrierCount = 1,
			.pImageMemoryBarriers = &IMAGE_MEMORY_BARRIER2,
		};

		VulkanPFNs::gpVkCmdPipelineBarrier2(commandBuffer, &PARENT_MEMORY_BARRIER2);
	}

	[[nodiscard]] VkImageView fGetImageView(uint32_t const& IMAGE_INDEX) {
		VkImageView returnImageView{};

		const VkImageViewCreateInfo IMAGE_VIEW_INFO{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = fGetSwapchainImages()[IMAGE_INDEX],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
		};

		CHECK_VK_SUCCESS(
		VulkanPFNs::gpVkCreateImageView(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, &IMAGE_VIEW_INFO, nullptr, &returnImageView),
		"Failed to create image view"
		)

		return returnImageView;
	}

	void fRecordDrawCommands(VkCommandBuffer& commandBuffer, uint32_t const& IMAGE_INDEX) {
		// rendering info/attachment info
		const VkImageView imageView{ fGetImageView(IMAGE_INDEX) };
		const VkRenderingAttachmentInfo COLOR_ATTACHMENT{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = imageView,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearColorValue({0.2f, 0.2f, 0.2f, 1.0f}),
		};
		const VkRenderingInfo RENDERING_INFO{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D(VkOffset2D(0, 0), gpVulkanSwapchainWrapper->mParameters.mSwapchainKHRCreateInfo.imageExtent),
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachments = &COLOR_ATTACHMENT,
		};

		// begin recording
		const VkCommandBufferBeginInfo BEGIN_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		};
		CHECK_VK_SUCCESS(
		VulkanPFNs::gpVkBeginCommandBuffer(commandBuffer, &BEGIN_INFO),
		"Failed to begin command buffer")
		// began recording

		// sets and bindings
		VulkanPFNs::gpVkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, gpVulkanGraphicsPipelineWrapper->mpGraphicsPipeline);
		const VkViewport VIEWPORT{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(gpVulkanSwapchainWrapper->mParameters.mSwapchainKHRCreateInfo.imageExtent.width),
			.height = static_cast<float>(gpVulkanSwapchainWrapper->mParameters.mSwapchainKHRCreateInfo.imageExtent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
		const VkRect2D SCISSOR{
			.offset = VkOffset2D(0, 0),
			.extent = gpVulkanSwapchainWrapper->mParameters.mSwapchainKHRCreateInfo.imageExtent
		};
		VulkanPFNs::gpVkCmdSetViewport(commandBuffer, 0, 1, &VIEWPORT);
		VulkanPFNs::gpVkCmdSetScissor(commandBuffer, 0, 1, &SCISSOR);
		constexpr VkDeviceSize ZERO_OFFSET{ 0 };
		VulkanPFNs::gpVkCmdBindVertexBuffers(commandBuffer, 0, 1, &gpVulkanDeviceLocalMemory->mDeviceLocalpBuffers[0], &ZERO_OFFSET);
		VulkanPFNs::gpVkCmdBindIndexBuffer(commandBuffer, gpVulkanDeviceLocalMemory->mDeviceLocalpBuffers[1], ZERO_OFFSET, VK_INDEX_TYPE_UINT32);

		// undefined/unknown layout -> color attachment output optimal
		fInsertImageMemoryBarrier2(commandBuffer, IMAGE_INDEX, VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
		VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpPhysicalDevice));
		
		// draw!
		VulkanPFNs::gpVkCmdBeginRendering(commandBuffer, &RENDERING_INFO);
		VulkanPFNs::gpVkCmdDrawIndexed(commandBuffer, 6, 1, 0, 0, 0);
		VulkanPFNs::gpVkCmdEndRendering(commandBuffer);

		// color attachment output optimal -> present optimal
		fInsertImageMemoryBarrier2(commandBuffer, IMAGE_INDEX, VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
		VK_ACCESS_2_NONE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpPhysicalDevice));

		// end recording
		CHECK_VK_SUCCESS(
		VulkanPFNs::gpVkEndCommandBuffer(commandBuffer),
		"Command buffer end recording failure"
		)
		// ended recording
	};

	void fAcquireNextSwapchainImageIndex(ImageKillhouse& killhouse, uint32_t& nextImageIndex) {
		uint32_t acquiredImageIndex{ UINT32_MAX };

		VkSemaphore pTemporarySemaphorePointer{};
		const VkSemaphoreCreateInfo EMPTY_INFO{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		};
		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkCreateSemaphore(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, &EMPTY_INFO, nullptr, &pTemporarySemaphorePointer),
			"Semaphore creation failed"
		)

		VulkanPFNs::gpVkAcquireNextImageKHR(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, gpVulkanSwapchainWrapper->mpSwapchainKHR, UINT64_MAX, pTemporarySemaphorePointer, VK_NULL_HANDLE, &acquiredImageIndex);
		killhouse.mHitmen[acquiredImageIndex].mpRenderReady = pTemporarySemaphorePointer;

		nextImageIndex = acquiredImageIndex;
	}

	void fSubmitDrawCommands(VkQueue& queue, ImageHitman& hitman) {
		const VkPipelineStageFlags WAIT_HERE{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		const VkSubmitInfo DRAW_COMMANDS_SUBMIT_INFO{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &hitman.mpRenderReady,
			.pWaitDstStageMask = &WAIT_HERE,
			.commandBufferCount = 1,
			.pCommandBuffers = &hitman.mDrawCommands,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &hitman.mpRenderFinished,
		};
		VulkanPFNs::gpVkQueueSubmit(queue, 1, &DRAW_COMMANDS_SUBMIT_INFO, hitman.mpOneAtATime);
	}

	void fQueueImageForPresentation(VkQueue& queue, uint32_t const& SWAPCHAIN_IMAGE_INDEX, ImageHitman& hitman) {
		const VkPresentInfoKHR QUEUE_PRESENT_INFO{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &hitman.mpRenderFinished,
			.swapchainCount = 1,
			.pSwapchains = &gpVulkanSwapchainWrapper->mpSwapchainKHR,
			.pImageIndices = &SWAPCHAIN_IMAGE_INDEX,
		};
		VulkanPFNs::gpVkQueuePresentKHR(queue, &QUEUE_PRESENT_INFO);	
	}

	void fRunThroughNextSwapchainImage(ImageKillhouse& killhouse) {
		uint32_t nextSwapchainImageIndex{ UINT32_MAX };
		fAcquireNextSwapchainImageIndex(killhouse, nextSwapchainImageIndex);

		ImageHitman& hitmanUsed{ killhouse.mHitmen[nextSwapchainImageIndex] };

		VulkanPFNs::gpVkWaitForFences(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, 1, &hitmanUsed.mpOneAtATime, VK_TRUE, UINT64_MAX);
		VulkanPFNs::gpVkResetFences(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice, 1, &hitmanUsed.mpOneAtATime);

		fRecordDrawCommands(hitmanUsed.mDrawCommands, nextSwapchainImageIndex);
		fSubmitDrawCommands(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mGraphicsFamilypQueues[0], hitmanUsed);
		fQueueImageForPresentation(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mGraphicsFamilypQueues[0], nextSwapchainImageIndex, hitmanUsed);
	}

	void fRenderLoop(ImageKillhouse& killhouse) {
		while(!glfwWindowShouldClose(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpVulkanBackendWrapper->mpGlfwWindowWrapper->mpGlfwWindow)) {
			glfwPollEvents();
			if(glfwGetKey(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpVulkanBackendWrapper->mpGlfwWindowWrapper->mpGlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpVulkanBackendWrapper->mpGlfwWindowWrapper->mpGlfwWindow, GLFW_TRUE);
			}

			fRunThroughNextSwapchainImage(killhouse);
		}

		VulkanPFNs::gpVkDeviceWaitIdle(gpVulkanSwapchainWrapper->mpVulkanDevicesWrapper->mpLogicalDevice);
	}
}
