#include <iostream>
#include <chrono>
#include "GlobalState.h"
#include "Engine.h"
#include "DeviceMemory.h"

#define CHECK_PRESSED(glfwKey) \
glfwGetKey(GlobalState::Core::getWindow().getGlfwWindow(), glfwKey) == GLFW_PRESS

namespace Engine {
	ImageHitman::ImageHitman(VkCommandPool pPool) :
	pRenderReady{},
	pRenderFinished{},
	pCommandPool{ pPool },
	pDrawCommands{} {
		const VkFenceCreateInfo FENCE_INFO{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		CHECK_VK_SUCCESS(
			vkCreateFence(GlobalState::Core::getDevices().getLogicalDevice(), &FENCE_INFO, nullptr, &pOneAtATime),
			"Fence creation failed"
		)

		const VkSemaphoreCreateInfo EMPTY_INFO{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
		};
		CHECK_VK_SUCCESS(
			vkCreateSemaphore(GlobalState::Core::getDevices().getLogicalDevice(), &EMPTY_INFO, nullptr, &pRenderReady),
			"Semaphore creation failed"
		)
		CHECK_VK_SUCCESS(
			vkCreateSemaphore(GlobalState::Core::getDevices().getLogicalDevice(), &EMPTY_INFO, nullptr, &pRenderFinished),
			"Semaphore creation failed"
		)

		const VkCommandBufferAllocateInfo DRAW_COMMANDS_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = pCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};

		CHECK_VK_SUCCESS(
			vkAllocateCommandBuffers(GlobalState::Core::getDevices().getLogicalDevice(), &DRAW_COMMANDS_INFO, &pDrawCommands),
			"Command buffer creation failed"
		)
	}

	ImageHitman::~ImageHitman() {
		vkDestroyFence(GlobalState::Core::getDevices().getLogicalDevice(), pOneAtATime, nullptr);
		vkDestroySemaphore(GlobalState::Core::getDevices().getLogicalDevice(), pRenderReady, nullptr);
		vkDestroySemaphore(GlobalState::Core::getDevices().getLogicalDevice(), pRenderFinished, nullptr);
		vkFreeCommandBuffers(GlobalState::Core::getDevices().getLogicalDevice(), pCommandPool, 1, &pDrawCommands);
	}

	ImageKillhouse::ImageKillhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX) : 
	pCommandPoolUsed{},
	hitmen{} {
		const VkCommandPoolCreateInfo POOL_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = GRAPHICS_QF_INDEX
		};

		CHECK_VK_SUCCESS(
			vkCreateCommandPool(GlobalState::Core::getDevices().getLogicalDevice(), &POOL_INFO, nullptr, &pCommandPoolUsed),
			"Failed to create command pool"
		)

		hitmen.reserve(HITMEN_COUNT);
		for(int i = 0; i < HITMEN_COUNT; i++) {
			hitmen.emplace_back(pCommandPoolUsed);
		}
	}

	ImageKillhouse::~ImageKillhouse() {
		hitmen.clear();
		vkDestroyCommandPool(GlobalState::Core::getDevices().getLogicalDevice(), pCommandPoolUsed, nullptr);
	}

	void ImageKillhouse::recreateHitmen() {
		const size_t PREVIOUS_HITMEN_COUNT{ hitmen.size() };

		hitmen.clear();
		hitmen.reserve(PREVIOUS_HITMEN_COUNT);
		for(int i = 0; i < PREVIOUS_HITMEN_COUNT; i++) {
			hitmen.emplace_back(pCommandPoolUsed);
		}
	}

	[[nodiscard]] std::vector<VkImage> getSwapchainImages() {
		uint32_t swapchainImageCount{};
		vkGetSwapchainImagesKHR(GlobalState::Core::getDevices().getLogicalDevice(), GlobalState::Core::getSwapchain().getSwapchain(), &swapchainImageCount, nullptr);
		std::vector<VkImage> swapchainImages(swapchainImageCount);
		vkGetSwapchainImagesKHR(GlobalState::Core::getDevices().getLogicalDevice(), GlobalState::Core::getSwapchain().getSwapchain(), &swapchainImageCount, swapchainImages.data());

		return swapchainImages;
	}

	[[nodiscard]] VkImageView getSwapchainImageView(uint32_t const& IMAGE_INDEX, VkFormat const& FORMAT, VkImageSubresourceRange const& SUBRESOURCE_RANGE) {
		VkImageView pReturnImageView{};

		const VkImageViewCreateInfo IMAGE_VIEW_INFO{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = getSwapchainImages()[IMAGE_INDEX],
			.viewType = VK_IMAGE_VIEW_TYPE_2D,
			.format = FORMAT,
			.subresourceRange = SUBRESOURCE_RANGE
		};

		CHECK_VK_SUCCESS(
		vkCreateImageView(GlobalState::Core::getDevices().getLogicalDevice(), &IMAGE_VIEW_INFO, nullptr, &pReturnImageView),
		"Failed to create image view"
		)

		return pReturnImageView;
	}

	void recordDrawCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX) {
		// rendering info/attachment info
		VkImageView pColorImageView{ getSwapchainImageView(IMAGE_INDEX, VK_FORMAT_R8G8B8A8_SRGB, VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)) };
		VkImageView pDepthImageView{ GlobalState::Core::getDeviceLocalMemory().getImageViews()[GlobalState::Core::getDeviceLocalMemory().searchForDepthImageIndex()] };
		const VkRenderingAttachmentInfo COLOR_ATTACHMENT{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = pColorImageView,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearColorValue({0.2f, 0.2f, 0.2f, 1.0f}),
		};
		const VkRenderingAttachmentInfo DEPTH_ATTACHMENT{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = pDepthImageView,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = {.depthStencil = VkClearDepthStencilValue(1.0f, 0) },
		};
		const VkRenderingInfo RENDERING_INFO{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D(VkOffset2D(0, 0), GlobalState::Core::getSwapchain().getCurrentExtent()),
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
		vkBeginCommandBuffer(pCommandBuffer, &BEGIN_INFO),
		"Failed to begin command buffer")
		// began recording

		// sets and bindings
		vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GlobalState::Core::getGraphicsPipeline().getGraphicsPipeline());
		const VkViewport VIEWPORT{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(GlobalState::Core::getSwapchain().getCurrentExtent().width),
			.height = static_cast<float>(GlobalState::Core::getSwapchain().getCurrentExtent().height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
		const VkRect2D SCISSOR{
			.offset = VkOffset2D(0, 0),
			.extent = GlobalState::Core::getSwapchain().getCurrentExtent()
		};
		vkCmdSetViewport(pCommandBuffer, 0, 1, &VIEWPORT);
		vkCmdSetScissor(pCommandBuffer, 0, 1, &SCISSOR);
		constexpr VkDeviceSize ZERO_OFFSET{ 0 };
		vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, &GlobalState::Core::getDeviceLocalMemory().getBuffers()[0], &ZERO_OFFSET);
		vkCmdBindIndexBuffer(pCommandBuffer, GlobalState::Core::getDeviceLocalMemory().getBuffers()[1], ZERO_OFFSET, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GlobalState::Core::getGraphicsPipeline().getCreateInfo().pGraphicsPipeline.layout, 0, 1, GlobalState::Core::getHostVisibleMemory().getDescriptorSets().data(), 0, nullptr);
		vkCmdBindDescriptorSets(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, GlobalState::Core::getGraphicsPipeline().getCreateInfo().pGraphicsPipeline.layout, 1, 1, GlobalState::Core::getDeviceLocalMemory().getDescriptorSets().data(), 0, nullptr);
			
		// layout transitions to optimal
		DeviceMemory::transitionImageLayout(pCommandBuffer, getSwapchainImages()[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
		VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, GlobalState::Core::getDevices().getGraphicsQfIndex());
		DeviceMemory::transitionImageLayout(pCommandBuffer, GlobalState::Core::getDeviceLocalMemory().getImages()[GlobalState::Core::getDeviceLocalMemory().searchForDepthImageIndex()], VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
		VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
		VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, GlobalState::Core::getDevices().getGraphicsQfIndex());

		// draw!
		vkCmdBeginRendering(pCommandBuffer, &RENDERING_INFO);

		vkCmdDrawIndexed(pCommandBuffer, GlobalState::Core::getGltfModel().second.size(), 1, 0, 0, 0);

		vkCmdEndRendering(pCommandBuffer);

		// layout transition to present optimal
		DeviceMemory::transitionImageLayout(pCommandBuffer, getSwapchainImages()[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
		VK_ACCESS_2_NONE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, GlobalState::Core::getDevices().getGraphicsQfIndex());

		// end recording
		CHECK_VK_SUCCESS(
		vkEndCommandBuffer(pCommandBuffer),
		"Command buffer end recording failure"
		)
		// ended recording

		vkDestroyImageView(GlobalState::Core::getDevices().getLogicalDevice(), pColorImageView, nullptr);
	};

	VkResult acquireNextSwapchainImageIndex(ImageKillhouse& killhouse, uint32_t& nextImageIndex) {
		const VkResult RESULT = vkAcquireNextImageKHR(GlobalState::Core::getDevices().getLogicalDevice(), GlobalState::Core::getSwapchain().getSwapchain(), UINT64_MAX, killhouse.hitmen[gHitmanIndex].pRenderReady, VK_NULL_HANDLE, &nextImageIndex);
		return RESULT;
	}

	void submitDrawCommands(VkQueue& pQueue, ImageHitman& hitman) {
		const VkPipelineStageFlags WAIT_HERE{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		const VkSubmitInfo DRAW_COMMANDS_SUBMIT_INFO{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &hitman.pRenderReady,
			.pWaitDstStageMask = &WAIT_HERE,
			.commandBufferCount = 1,
			.pCommandBuffers = &hitman.pDrawCommands,
			.signalSemaphoreCount = 1,
			.pSignalSemaphores = &hitman.pRenderFinished,
		};
		vkQueueSubmit(pQueue, 1, &DRAW_COMMANDS_SUBMIT_INFO, hitman.pOneAtATime);
	}

	VkResult queueImageForPresentation(VkQueue& pQueue, uint32_t const& SWAPCHAIN_IMAGE_INDEX, ImageHitman& hitman) {
		const VkPresentInfoKHR QUEUE_PRESENT_INFO{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.waitSemaphoreCount = 1,
			.pWaitSemaphores = &hitman.pRenderFinished,
			.swapchainCount = 1,
			.pSwapchains = &GlobalState::Core::getSwapchain().getSwapchain(),
			.pImageIndices = &SWAPCHAIN_IMAGE_INDEX,
		};
		return vkQueuePresentKHR(pQueue, &QUEUE_PRESENT_INFO);	
	}

	bool recreateSwapchainIfNecessary(VkResult const& RESULT) {
		bool resized{ false };

		if(RESULT == VK_ERROR_OUT_OF_DATE_KHR || GlobalState::Core::getWindow().framebufferResized) {
			vkDeviceWaitIdle(GlobalState::Core::getDevices().getLogicalDevice());
			GlobalState::Core::getSwapchain().recreate();
			GlobalState::Core::getDeviceLocalMemory().recreateDepthResources();
			GlobalState::Core::getWindow().framebufferResized = false;

			resized = true;
		} else if(RESULT != VK_SUCCESS) {
			throw std::runtime_error("Rendering operation did not return VK_SUCCESS nor VK_ERROR_OUT_OF_DATE_KHR");
		}

		return resized;
	}

	void initializeCurrentTransformation() {
		gCurrentTransformation = Vertex::Transforms(
			glm::mat4{1.0f},
			glm::mat4{glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f))},
			glm::mat4{glm::perspective(glm::radians(45.0f), static_cast<float>(GlobalState::Core::getSwapchain().getCurrentExtent().width) / static_cast<float>(GlobalState::Core::getSwapchain().getCurrentExtent().height), 0.1f, 1000.0f)}
		);
		gCurrentTransformation.projection[1][1] *= -1.0f;
	}

	[[nodiscard]] float getTimeSinceFirstCall() {
		const static std::chrono::steady_clock::time_point FIRST_CALL_TIME = std::chrono::high_resolution_clock::now();
		const std::chrono::steady_clock::time_point THIS_CALL_TIME = std::chrono::high_resolution_clock::now();
		const float TIME_SINCE_FIRST_CALL = std::chrono::duration<float, std::chrono::seconds::period>(THIS_CALL_TIME - FIRST_CALL_TIME).count();

		return TIME_SINCE_FIRST_CALL;
	}

	void reactToInput() {
		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_X)) {
			gCurrentTransformation.model = glm::translate(gCurrentTransformation.model, glm::vec3(-0.001f, 0.0f, 0.0f));
		} else if(CHECK_PRESSED(GLFW_KEY_X)) {
			gCurrentTransformation.model = glm::translate(gCurrentTransformation.model, glm::vec3(0.001f, 0.0f, 0.0f));
		}
		
		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_Y)) {
			gCurrentTransformation.model = glm::translate(gCurrentTransformation.model, glm::vec3(0.0f, -0.001f, 0.0f));
		} else if(CHECK_PRESSED(GLFW_KEY_Y)) {
			gCurrentTransformation.model = glm::translate(gCurrentTransformation.model, glm::vec3(0.0f, 0.001f, 0.0f));
		}

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_Z)) {
			gCurrentTransformation.model = glm::translate(gCurrentTransformation.model, glm::vec3(0.0f, 0.0f, -0.001f));
		} else if(CHECK_PRESSED(GLFW_KEY_Z)) {
			gCurrentTransformation.model = glm::translate(gCurrentTransformation.model, glm::vec3(0.0f, 0.0f, 0.001f));
		}

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_S)) {
			gCurrentTransformation.model = glm::scale(gCurrentTransformation.model, glm::vec3(0.999f, 0.999f, 0.999f));
		} else if(CHECK_PRESSED(GLFW_KEY_S)) {
			gCurrentTransformation.model = glm::scale(gCurrentTransformation.model, glm::vec3(1.001f, 1.001f, 1.001f));
		}
	}

	void writeToUniformBuffer() {
		GlobalState::Core::getHostVisibleMemory().writeToBuffer(2, &gCurrentTransformation, sizeof(Vertex::Transforms));
	}

	void runThroughNextSwapchainImage(ImageKillhouse& killhouse) {
		ImageHitman& hitmanUsed{ killhouse.hitmen[gHitmanIndex] };
		uint32_t nextSwapchainImageIndex{ UINT32_MAX };

		vkWaitForFences(GlobalState::Core::getDevices().getLogicalDevice(), 1, &hitmanUsed.pOneAtATime, VK_TRUE, UINT64_MAX);
		const VkResult ACQUIRE_RESULT{ acquireNextSwapchainImageIndex(killhouse, nextSwapchainImageIndex) };
		if(recreateSwapchainIfNecessary(ACQUIRE_RESULT)) {
			killhouse.recreateHitmen();
			gHitmanIndex = 0;
			return;
		}

		vkResetFences(GlobalState::Core::getDevices().getLogicalDevice(), 1, &hitmanUsed.pOneAtATime);
		vkResetCommandBuffer(hitmanUsed.pDrawCommands, 0);

		recordDrawCommands(hitmanUsed.pDrawCommands, nextSwapchainImageIndex);
		reactToInput();
		writeToUniformBuffer();
		submitDrawCommands(GlobalState::Core::getDevices().getGraphicsQueues()[0], hitmanUsed);
		const VkResult PRESENT_RESULT{ queueImageForPresentation(GlobalState::Core::getDevices().getGraphicsQueues()[0], nextSwapchainImageIndex, hitmanUsed) };
		if(recreateSwapchainIfNecessary(PRESENT_RESULT)) {
			killhouse.recreateHitmen();
			gHitmanIndex = 0;
			return;
		}

		gHitmanIndex = (gHitmanIndex + 1) % static_cast<uint32_t>(killhouse.hitmen.size());;
	}

	void renderLoop(ImageKillhouse& killhouse) {
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
		initializeCurrentTransformation();

		while(!glfwWindowShouldClose(GlobalState::Core::getWindow().getGlfwWindow())) {
			glfwPollEvents();
			if(glfwGetKey(GlobalState::Core::getWindow().getGlfwWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(GlobalState::Core::getWindow().getGlfwWindow(), GLFW_TRUE);
			}

			runThroughNextSwapchainImage(killhouse);
			incrementFramesCountAndCheck();
		}

		vkDeviceWaitIdle(GlobalState::Core::getDevices().getLogicalDevice());
	}
}
