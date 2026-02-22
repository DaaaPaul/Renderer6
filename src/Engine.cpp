#include <iostream>
#include <chrono>
#include "Engine.h"
#include "DeviceMemoryCommon.h"

#define CHECK_PRESSED(glfwKey) \
glfwGetKey(GlobalState::Core::getWindow().glfwWindow, glfwKey) == GLFW_PRESS

namespace Engine {
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
				vkCreateFence(GlobalState::Core::getDevices().logicalDevice, &FENCE_INFO, nullptr, &mpOneAtATime),
				"Fence creation failed"
			)
		}

		// create semaphores
		{
			const VkSemaphoreCreateInfo EMPTY_INFO{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};
			CHECK_VK_SUCCESS(
				vkCreateSemaphore(GlobalState::Core::getDevices().logicalDevice, &EMPTY_INFO, nullptr, &mpRenderReady),
				"Semaphore creation failed"
			)
			CHECK_VK_SUCCESS(
				vkCreateSemaphore(GlobalState::Core::getDevices().logicalDevice, &EMPTY_INFO, nullptr, &mpRenderFinished),
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
				vkAllocateCommandBuffers(GlobalState::Core::getDevices().logicalDevice, &DRAW_COMMANDS_INFO, &mDrawCommands),
				"Command buffer creation failed"
			)
		}
	}

	ImageHitman::~ImageHitman() {
		vkDestroyFence(GlobalState::Core::getDevices().logicalDevice, mpOneAtATime, nullptr);
		vkDestroySemaphore(GlobalState::Core::getDevices().logicalDevice, mpRenderReady, nullptr);
		vkDestroySemaphore(GlobalState::Core::getDevices().logicalDevice, mpRenderFinished, nullptr);
		vkFreeCommandBuffers(GlobalState::Core::getDevices().logicalDevice, mpCommandPool, 1, &mDrawCommands);
	}

	ImageKillhouse::ImageKillhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX) : 
	mpCommandPoolUsed{},
	mHitmen{} {
		const VkCommandPoolCreateInfo POOL_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = GRAPHICS_QF_INDEX
		};

		CHECK_VK_SUCCESS(
			vkCreateCommandPool(GlobalState::Core::getDevices().logicalDevice, &POOL_INFO, nullptr, &mpCommandPoolUsed),
			"Failed to create command pool"
		)

		mHitmen.reserve(HITMEN_COUNT);
		for(int i = 0; i < HITMEN_COUNT; i++) {
			mHitmen.emplace_back(mpCommandPoolUsed);
		}
	}

	ImageKillhouse::~ImageKillhouse() {
		mHitmen.clear();
		vkDestroyCommandPool(GlobalState::Core::getDevices().logicalDevice, mpCommandPoolUsed, nullptr);
	}

	void ImageKillhouse::recreateHitmen() {
		const size_t PREVIOUS_HITMEN_COUNT{ mHitmen.size() };

		mHitmen.clear();
		mHitmen.reserve(PREVIOUS_HITMEN_COUNT);
		for(int i = 0; i < PREVIOUS_HITMEN_COUNT; i++) {
			mHitmen.emplace_back(mpCommandPoolUsed);
		}
	}

	[[nodiscard]] std::vector<VkImage> fGetSwapchainImages() {
		uint32_t swapchainImageCount{};
		vkGetSwapchainImagesKHR(GlobalState::Core::getDevices().logicalDevice, GlobalState::Core::getSwapchain().swapchain, &swapchainImageCount, nullptr);
		std::vector<VkImage> swapchainImages(swapchainImageCount);
		vkGetSwapchainImagesKHR(GlobalState::Core::getDevices().logicalDevice, GlobalState::Core::getSwapchain().swapchain, &swapchainImageCount, swapchainImages.data());

		return swapchainImages;
	}

	[[nodiscard]] VkImageView fGetSwapchainImageView(uint32_t const& IMAGE_INDEX) {
		VkImageView returnImageView{};

		const VkImageViewCreateInfo IMAGE_VIEW_INFO{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = fGetSwapchainImages()[IMAGE_INDEX],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = VK_FORMAT_R8G8B8A8_SRGB,
			.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
		};

		CHECK_VK_SUCCESS(
		vkCreateImageView(GlobalState::Core::getDevices().logicalDevice, &IMAGE_VIEW_INFO, nullptr, &returnImageView),
		"Failed to create image view"
		)

		return returnImageView;
	}

	void fRecordDrawCommands(VkCommandBuffer& commandBuffer, uint32_t const& IMAGE_INDEX) {
		// rendering info/attachment info
		VkImageView colorImageView{ fGetSwapchainImageView(IMAGE_INDEX) };
		VkImageView depthImageView{ GlobalState::Core::getDeviceLocalMemory().imageViews[1] };
		const VkRenderingAttachmentInfo COLOR_ATTACHMENT{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = colorImageView,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearColorValue({0.2f, 0.2f, 0.2f, 1.0f}),
		};
		const VkRenderingAttachmentInfo DEPTH_ATTACHMENT{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = depthImageView,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = {.depthStencil = VkClearDepthStencilValue(1.0f, 0) },
		};
		const VkRenderingInfo RENDERING_INFO{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D(VkOffset2D(0, 0), GlobalState::Core::getSwapchain().CREATE_INFO.createInfo.imageExtent),
			.layerCount = 1,
			.viewMask = 0,
			.colorAttachmentCount = 1,
			.pColorAttachments = &COLOR_ATTACHMENT,
			.pDepthAttachment = &DEPTH_ATTACHMENT
		};

		// begin recording
		const VkCommandBufferBeginInfo BEGIN_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		};
		CHECK_VK_SUCCESS(
		vkBeginCommandBuffer(commandBuffer, &BEGIN_INFO),
		"Failed to begin command buffer")
		// began recording

		// sets and bindings
		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GlobalState::Core::getGraphicsPipeline().mpGraphicsPipeline);
		const VkViewport VIEWPORT{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(GlobalState::Core::getSwapchain().CREATE_INFO.createInfo.imageExtent.width),
			.height = static_cast<float>(GlobalState::Core::getSwapchain().CREATE_INFO.createInfo.imageExtent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
		const VkRect2D SCISSOR{
			.offset = VkOffset2D(0, 0),
			.extent = GlobalState::Core::getSwapchain().CREATE_INFO.createInfo.imageExtent
		};
		vkCmdSetViewport(commandBuffer, 0, 1, &VIEWPORT);
		vkCmdSetScissor(commandBuffer, 0, 1, &SCISSOR);
		constexpr VkDeviceSize ZERO_OFFSET{ 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, &GlobalState::Core::getDeviceLocalMemory().buffers[0], &ZERO_OFFSET);
		vkCmdBindIndexBuffer(commandBuffer, GlobalState::Core::getDeviceLocalMemory().buffers[1], ZERO_OFFSET, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GlobalState::Core::getGraphicsPipeline().CREATE_INFO.mpPipelineLayout, 0, 1, GlobalState::Core::getHostVisibleMemory().descriptorSets.data(), 0, nullptr);
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GlobalState::Core::getGraphicsPipeline().CREATE_INFO.mpPipelineLayout, 1, 1, GlobalState::Core::getDeviceLocalMemory().descriptorSets.data(), 0, nullptr);
			
		// layout transitions to optimal
		DeviceMemory::Common::fTransitionImageLayout(commandBuffer, fGetSwapchainImages()[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
		VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, GlobalState::Core::getDevices().getGraphicsQfIndex());
		DeviceMemory::Common::fTransitionImageLayout(commandBuffer, GlobalState::Core::getDeviceLocalMemory().images[1], VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
		VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
		VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, GlobalState::Core::getDevices().getGraphicsQfIndex());

		// draw!
		vkCmdBeginRendering(commandBuffer, &RENDERING_INFO);

		vkCmdDrawIndexed(commandBuffer, 12, 1, 0, 0, 0);

		vkCmdEndRendering(commandBuffer);

		// layout transition to present optimal
		DeviceMemory::Common::fTransitionImageLayout(commandBuffer, fGetSwapchainImages()[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
		VK_ACCESS_2_NONE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, GlobalState::Core::getDevices().getGraphicsQfIndex());

		// end recording
		CHECK_VK_SUCCESS(
		vkEndCommandBuffer(commandBuffer),
		"Command buffer end recording failure"
		)
		// ended recording

		vkDestroyImageView(GlobalState::Core::getDevices().logicalDevice, colorImageView, nullptr);
	};

	const VkResult fAcquireNextSwapchainImageIndex(ImageKillhouse& killhouse, uint32_t& nextImageIndex) {
		const VkResult RESULT = vkAcquireNextImageKHR(GlobalState::Core::getDevices().logicalDevice, GlobalState::Core::getSwapchain().swapchain, UINT64_MAX, killhouse.mHitmen[gHitmanIndex].mpRenderReady, VK_NULL_HANDLE, &nextImageIndex);
		return RESULT;
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
		vkQueueSubmit(queue, 1, &DRAW_COMMANDS_SUBMIT_INFO, hitman.mpOneAtATime);
	}

	const VkResult fQueueImageForPresentation(VkQueue& queue, uint32_t const& SWAPCHAIN_IMAGE_INDEX, ImageHitman& hitman) {
		const VkPresentInfoKHR QUEUE_PRESENT_INFO{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &hitman.mpRenderFinished,
			.swapchainCount = 1,
			.pSwapchains = &GlobalState::Core::getSwapchain().swapchain,
			.pImageIndices = &SWAPCHAIN_IMAGE_INDEX,
		};
		return vkQueuePresentKHR(queue, &QUEUE_PRESENT_INFO);	
	}

	const bool fRecreateSwapchainIfNecessary(VkResult const& RESULT) {
		bool resized{ false };

		if(RESULT == VK_ERROR_OUT_OF_DATE_KHR || GlobalState::Core::getWindow().framebufferResized) {
			vkDeviceWaitIdle(GlobalState::Core::getDevices().logicalDevice);
			GlobalState::Core::getSwapchain().recreateThyself();
			GlobalState::Core::getWindow().framebufferResized = false;

			resized = true;
		} else if(RESULT != VK_SUCCESS) {
			throw std::runtime_error("Rendering operation did not return VK_SUCCESS nor VK_ERROR_OUT_OF_DATE_KHR");
		}

		return resized;
	}

	void fInitializegCurrentTransformation() {
		gCurrentTransformation = Vertex::Transforms(
			glm::mat4{1.0f},
			glm::mat4{glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))},
			glm::mat4{glm::perspective(glm::radians(45.0f), static_cast<float>(GlobalState::Core::getSwapchain().CREATE_INFO.createInfo.imageExtent.width) / static_cast<float>(GlobalState::Core::getSwapchain().CREATE_INFO.createInfo.imageExtent.height), 0.1f, 1000.0f)}
		);
		gCurrentTransformation.mProjection[1][1] *= -1.0f;
	}

	[[nodiscard]] const float fGetTimeSinceFirstCall() {
		const static std::chrono::steady_clock::time_point sFIRST_CALL_TIME = std::chrono::high_resolution_clock::now();
		const std::chrono::steady_clock::time_point THIS_CALL_TIME = std::chrono::high_resolution_clock::now();
		const float TIME_SINCE_FIRST_CALL = std::chrono::duration<float, std::chrono::seconds::period>(THIS_CALL_TIME - sFIRST_CALL_TIME).count();

		return TIME_SINCE_FIRST_CALL;
	}

	void fReactToInput() {
		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_X)) {
			gCurrentTransformation.mModel = glm::translate(gCurrentTransformation.mModel, glm::vec3(-0.001f, 0.0f, 0.0f));
		} else if(CHECK_PRESSED(GLFW_KEY_X)) {
			gCurrentTransformation.mModel = glm::translate(gCurrentTransformation.mModel, glm::vec3(0.001f, 0.0f, 0.0f));
		}
		
		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_Y)) {
			gCurrentTransformation.mModel = glm::translate(gCurrentTransformation.mModel, glm::vec3(0.0f, -0.001f, 0.0f));
		} else if(CHECK_PRESSED(GLFW_KEY_Y)) {
			gCurrentTransformation.mModel = glm::translate(gCurrentTransformation.mModel, glm::vec3(0.0f, 0.001f, 0.0f));
		}

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_Z)) {
			gCurrentTransformation.mModel = glm::translate(gCurrentTransformation.mModel, glm::vec3(0.0f, 0.0f, -0.001f));
		} else if(CHECK_PRESSED(GLFW_KEY_Z)) {
			gCurrentTransformation.mModel = glm::translate(gCurrentTransformation.mModel, glm::vec3(0.0f, 0.0f, 0.001f));
		}
	}

	void fWriteToUniformBuffer() {
		GlobalState::Core::getHostVisibleMemory().writeToBuffer(2, &gCurrentTransformation, sizeof(Vertex::Transforms));
	}

	void fRunThroughNextSwapchainImage(ImageKillhouse& killhouse) {
		ImageHitman& hitmanUsed{ killhouse.mHitmen[gHitmanIndex] };
		uint32_t nextSwapchainImageIndex{ UINT32_MAX };

		vkWaitForFences(GlobalState::Core::getDevices().logicalDevice, 1, &hitmanUsed.mpOneAtATime, VK_TRUE, UINT64_MAX);
		const VkResult ACQUIRE_RESULT{ fAcquireNextSwapchainImageIndex(killhouse, nextSwapchainImageIndex) };
		if(fRecreateSwapchainIfNecessary(ACQUIRE_RESULT)) {
			killhouse.recreateHitmen();
			gHitmanIndex = 0;
			return;
		}

		vkResetFences(GlobalState::Core::getDevices().logicalDevice, 1, &hitmanUsed.mpOneAtATime);
		vkResetCommandBuffer(hitmanUsed.mDrawCommands, 0);

		fRecordDrawCommands(hitmanUsed.mDrawCommands, nextSwapchainImageIndex);
		fReactToInput();
		fWriteToUniformBuffer();
		fSubmitDrawCommands(GlobalState::Core::getDevices().graphicsQueues[0], hitmanUsed);
		const VkResult PRESENT_RESULT{ fQueueImageForPresentation(GlobalState::Core::getDevices().graphicsQueues[0], nextSwapchainImageIndex, hitmanUsed) };
		if(fRecreateSwapchainIfNecessary(PRESENT_RESULT)) {
			killhouse.recreateHitmen();
			gHitmanIndex = 0;
			return;
		}

		gHitmanIndex = (gHitmanIndex + 1) % static_cast<uint32_t>(killhouse.mHitmen.size());;
	}

	void fRenderLoop(ImageKillhouse& killhouse) {
		uint16_t nextSecondMark{ 1 };
		uint16_t accumulatedFramesCount{ 0 };

		auto incrementFramesCountAndCheck = [&accumulatedFramesCount, &nextSecondMark]() -> void {
			accumulatedFramesCount++;
			if(glfwGetTime() > nextSecondMark) {
				nextSecondMark++;
				std::cout << "Frames last second: " << accumulatedFramesCount << '\n';
				accumulatedFramesCount = 0;
			}
		};
		fInitializegCurrentTransformation();

		while(!glfwWindowShouldClose(GlobalState::Core::getWindow().glfwWindow)) {
			glfwPollEvents();
			if(glfwGetKey(GlobalState::Core::getWindow().glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(GlobalState::Core::getWindow().glfwWindow, GLFW_TRUE);
			}

			fRunThroughNextSwapchainImage(killhouse);
			incrementFramesCountAndCheck();
		}

		vkDeviceWaitIdle(GlobalState::Core::getDevices().logicalDevice);
	}
}
