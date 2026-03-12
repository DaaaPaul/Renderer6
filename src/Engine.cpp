#include <iostream>
#include <chrono>
#include "Global.h"
#include "Engine.h"
#include "DeviceMemory.h"

#define CHECK_PRESSED(glfwKey) \
	glfwGetKey(Global::getWindow().getGlfwWindow(), glfwKey) == GLFW_PRESS

namespace Engine {
	Hitman::Hitman(VkCommandPool pPool) :
	pOneAtATime{},
	pTimeline{},
	pCommandPool{ pPool },
	pDrawCommands{} {
		VkFenceCreateInfo signedFenceCreate{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			.flags = VK_FENCE_CREATE_SIGNALED_BIT
		};
		CHECK_VK_SUCCESS(
			vkCreateFence(Global::getDevices().getLogicalDevice(), &signedFenceCreate, nullptr, &pOneAtATime),
			"Fence creation failed"
		)

		VkSemaphoreTypeCreateInfo timelineType{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
			.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
			.initialValue = 0
		};
		VkSemaphoreCreateInfo timelineSemaphoreInfo{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			.pNext = &timelineSemaphoreInfo
		};
		CHECK_VK_SUCCESS(
			vkCreateSemaphore(Global::getDevices().getLogicalDevice(), &timelineSemaphoreInfo, nullptr, &pTimeline),
			"Semaphore creation failed"
		)

		VkCommandBufferAllocateInfo commandBufferInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = pCommandPool,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		CHECK_VK_SUCCESS(
			vkAllocateCommandBuffers(Global::getDevices().getLogicalDevice(), &commandBufferInfo, &pDrawCommands),
			"Command buffer creation failed"
		)
		CHECK_VK_SUCCESS(
			vkAllocateCommandBuffers(Global::getDevices().getLogicalDevice(), &commandBufferInfo, &pComputeCommands),
			"Command buffer creation failed"
		)
	}

	Hitman::~Hitman() {
		vkDestroyFence(Global::getDevices().getLogicalDevice(), pOneAtATime, nullptr);
		vkDestroySemaphore(Global::getDevices().getLogicalDevice(), pTimeline, nullptr);
		vkFreeCommandBuffers(Global::getDevices().getLogicalDevice(), pCommandPool, 1, &pDrawCommands);
		vkFreeCommandBuffers(Global::getDevices().getLogicalDevice(), pCommandPool, 1, &pComputeCommands);
	}

	Killhouse::Killhouse() :
		pCommandPoolUsed{},
		hitmen{} {
	
	}

	Killhouse::Killhouse(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX) : 
		pCommandPoolUsed{},
		hitmen{} {
		VkCommandPoolCreateInfo poolCreate{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = GRAPHICS_QF_INDEX
		};

		CHECK_VK_SUCCESS(
			vkCreateCommandPool(Global::getDevices().getLogicalDevice(), &poolCreate, nullptr, &pCommandPoolUsed),
			"Failed to create command pool"
		)

		hitmen.reserve(HITMEN_COUNT);
		for(int i = 0; i < HITMEN_COUNT; i++) {
			hitmen.emplace_back(pCommandPoolUsed);
		}
	}

	Killhouse::~Killhouse() {
		hitmen.clear();
		vkDestroyCommandPool(Global::getDevices().getLogicalDevice(), pCommandPoolUsed, nullptr);
	}

	void Killhouse::recreateHitmen() {
		const size_t PREVIOUS_HITMEN_COUNT{ hitmen.size() };

		hitmen.clear();
		hitmen.reserve(PREVIOUS_HITMEN_COUNT);
		for(int i = 0; i < PREVIOUS_HITMEN_COUNT; i++) {
			hitmen.emplace_back(pCommandPoolUsed);
		}
	}

	/* ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------ */

	[[nodiscard]] float getDeltaTime() noexcept {
		static std::chrono::steady_clock::time_point previousCallTime = std::chrono::high_resolution_clock::now();
		std::chrono::steady_clock::time_point nowTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(nowTime - previousCallTime).count();
		previousCallTime = nowTime;

		return deltaTime;
	}

	void recordDrawCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX) {
		using namespace Global;
		vkResetCommandBuffer(pCommandBuffer, 0);

		// rendering info/attachment info
		VkImageView pColorImageView = DeviceMemory::createImageView(getDevices().getLogicalDevice(), getSwapchainImages()[IMAGE_INDEX], 
			DeviceMemory::ImageViewInfo{
				.type = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_R8G8B8A8_SRGB,
				.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
			});
		VkImageView pDepthImageView = getDeviceLocalMemory().getImageViews()[getDeviceLocalMemory().searchForDepthImageIndex()];
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
			.renderArea = VkRect2D(VkOffset2D(0, 0), getSwapchain().getCurrentExtent()),
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
		vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, getGraphicsPipeline().getGraphicsPipeline());
		const VkViewport VIEWPORT{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(getSwapchain().getCurrentExtent().width),
			.height = static_cast<float>(getSwapchain().getCurrentExtent().height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
		const VkRect2D SCISSOR{
			.offset = VkOffset2D(0, 0),
			.extent = getSwapchain().getCurrentExtent()
		};
		vkCmdSetViewport(pCommandBuffer, 0, 1, &VIEWPORT);
		vkCmdSetScissor(pCommandBuffer, 0, 1, &SCISSOR);
		const std::vector<VkDeviceSize> ZERO{ 0 };
		vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, &getDeviceLocalMemory().getBuffers()[0], ZERO.data());
		vkCmdBindIndexBuffer(pCommandBuffer, getDeviceLocalMemory().getBuffers()[1], 0, VK_INDEX_TYPE_UINT32);
		vkCmdBindDescriptorSets(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, getGraphicsPipeline().getCreateInfo().pipelineInfo.layout, 
			0, 1, getHostVisibleMemory().getDescriptorSets().data(), 0, nullptr);
		vkCmdBindDescriptorSets(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, getGraphicsPipeline().getCreateInfo().pipelineInfo.layout, 
			1, 1, getDeviceLocalMemory().getDescriptorSets().data(), 0, nullptr);
			
		// layout transitions to optimal
		DeviceMemory::transitionImageLayout(pCommandBuffer, getSwapchainImages()[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
		VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, getDevices().getGraphicsQfIndex());
		DeviceMemory::transitionImageLayout(pCommandBuffer, getDeviceLocalMemory().getImages()[getDeviceLocalMemory().searchForDepthImageIndex()], VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
		VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT,
		VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, getDevices().getGraphicsQfIndex());

		// draw!
		vkCmdBeginRendering(pCommandBuffer, &RENDERING_INFO);

		vkCmdDrawIndexed(pCommandBuffer, getGltfModel().second.size(), 1, 0, 0, 0);

		vkCmdEndRendering(pCommandBuffer);

		// layout transition to present optimal
		DeviceMemory::transitionImageLayout(pCommandBuffer, getSwapchainImages()[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
		VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
		VK_ACCESS_2_NONE, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, getDevices().getGraphicsQfIndex());

		// end recording
		CHECK_VK_SUCCESS(
		vkEndCommandBuffer(pCommandBuffer),
		"Command buffer end recording failure"
		)
		// ended recording

		vkDestroyImageView(getDevices().getLogicalDevice(), pColorImageView, nullptr);
	};

	void recordComputeCommands(VkCommandBuffer& pCommandBuffer) {
		vkResetCommandBuffer(pCommandBuffer, 0);

		const VkCommandBufferBeginInfo BEGIN_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		};
		CHECK_VK_SUCCESS(
			vkBeginCommandBuffer(pCommandBuffer, &BEGIN_INFO),
			"Failed to begin compute command buffer"
		)
		
		vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Global::getComputePipeline().getComputePipeline());

		std::vector<VkDescriptorSet> computeDescriptorSets{};
		computeDescriptorSets.push_back(Global::getHostVisibleMemory().getDescriptorSets()[1]);
		computeDescriptorSets.push_back(Global::getDeviceLocalMemory().getDescriptorSets()[1]);
		computeDescriptorSets.push_back(Global::getDeviceLocalMemory().getDescriptorSets()[2]);
		vkCmdBindDescriptorSets(pCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Global::getComputePipeline().getCreateInfo().pipelineInfo.layout, 
			2, 3, computeDescriptorSets.data(), 0, nullptr);
			
		assert((Global::getParticlesData().size() % 256) == 0);
		vkCmdDispatch(pCommandBuffer, Global::getParticlesData().size() / 256, 1, 1);

		CHECK_VK_SUCCESS(
			vkEndCommandBuffer(pCommandBuffer),
			"Failed to end compute command buffer"
		)
	}

	void windowResizeRecreate() {
		using namespace Global::Engine;

		vkDeviceWaitIdle(Global::getDevices().getLogicalDevice());

		Global::getSwapchain().recreate();
		Global::getDeviceLocalMemory().recreateDepthResources();
		Global::getWindow().framebufferResized = false;
		getKillhouse().recreateHitmen();
		gHitmanIndex = 0;
	}

	void freshenTransformation() {
		using namespace Global::Engine;
		bool updatedCurrentTransformation = false;

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_X)) {
			getCurrentTransformation().model = glm::translate(getCurrentTransformation().model, glm::vec3(-0.001f, 0.0f, 0.0f));
			updatedCurrentTransformation = true;
		} else if(CHECK_PRESSED(GLFW_KEY_X)) {
			getCurrentTransformation().model = glm::translate(getCurrentTransformation().model, glm::vec3(0.001f, 0.0f, 0.0f));
			updatedCurrentTransformation = true;
		}
		
		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_Y)) {
			getCurrentTransformation().model = glm::translate(getCurrentTransformation().model, glm::vec3(0.0f, -0.001f, 0.0f));
			updatedCurrentTransformation = true;
		} else if(CHECK_PRESSED(GLFW_KEY_Y)) {
			getCurrentTransformation().model = glm::translate(getCurrentTransformation().model, glm::vec3(0.0f, 0.001f, 0.0f));
			updatedCurrentTransformation = true;
		}

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_Z)) {
			getCurrentTransformation().model = glm::translate(getCurrentTransformation().model, glm::vec3(0.0f, 0.0f, -0.001f));
			updatedCurrentTransformation = true;
		} else if(CHECK_PRESSED(GLFW_KEY_Z)) {
			getCurrentTransformation().model = glm::translate(getCurrentTransformation().model, glm::vec3(0.0f, 0.0f, 0.001f));
			updatedCurrentTransformation = true;
		}

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_S)) {
			getCurrentTransformation().model = glm::scale(getCurrentTransformation().model, glm::vec3(0.999f, 0.999f, 0.999f));
			updatedCurrentTransformation = true;
		} else if(CHECK_PRESSED(GLFW_KEY_S)) {
			getCurrentTransformation().model = glm::scale(getCurrentTransformation().model, glm::vec3(1.001f, 1.001f, 1.001f));
			updatedCurrentTransformation = true;
		}

		if(updatedCurrentTransformation) {
			Global::getHostVisibleMemory().writeToBuffer(4, &Global::Engine::getCurrentTransformation(), sizeof(Vertex::Transforms));
		}
	}

	void runNextSwapchainImage() {
		using namespace Global::Engine;
		Hitman& hitman = getKillhouse().hitmen[gHitmanIndex];
		uint64_t computeWaitVal = gTimelineValue;
		uint64_t computeSignalVal = ++gTimelineValue;
		uint64_t drawWaitVal = computeSignalVal;
		uint64_t drawSignalVal = ++gTimelineValue;
		uint32_t nextImageIndex = 0xFFFFFFFF;

		if(vkAcquireNextImageKHR(Global::getDevices().getLogicalDevice(), Global::getSwapchain().getSwapchain(), UINT64_MAX, VK_NULL_HANDLE, hitman.pOneAtATime, &nextImageIndex) == VK_ERROR_OUT_OF_DATE_KHR || Global::getWindow().framebufferResized) {
			windowResizeRecreate();
			return;
		}
		vkWaitForFences(Global::getDevices().getLogicalDevice(), 1, &hitman.pOneAtATime, VK_TRUE, UINT64_MAX);
		vkResetFences(Global::getDevices().getLogicalDevice(), 1, &hitman.pOneAtATime);

		// compute submit info
		recordComputeCommands(hitman.pComputeCommands);
		VkSemaphoreSubmitInfo computeWait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = hitman.pTimeline,
			.value = computeWaitVal,
			.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
		};
		VkCommandBufferSubmitInfo computeCommands{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = hitman.pComputeCommands,
		};
		VkSemaphoreSubmitInfo computeSignal{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = hitman.pTimeline,
			.value = computeSignalVal,
			.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
		};
		VkSubmitInfo2 computeSubmit{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = 1,
			.pWaitSemaphoreInfos = &computeWait,
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &computeCommands,
			.signalSemaphoreInfoCount = 1,
			.pSignalSemaphoreInfos = &computeSignal
		};

		// graphics submit info
		recordDrawCommands(hitman.pDrawCommands, nextImageIndex);
		freshenTransformation();
		VkSemaphoreSubmitInfo drawWait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = hitman.pTimeline,
			.value = drawWaitVal,
			.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
		};
		VkCommandBufferSubmitInfo drawCommands{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = hitman.pDrawCommands,
		};
		VkSemaphoreSubmitInfo drawSignal{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = hitman.pTimeline,
			.value = drawSignalVal,
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		};
		VkSubmitInfo2 drawSubmit{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = 1,
			.pWaitSemaphoreInfos = &drawWait,
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &drawCommands,
			.signalSemaphoreInfoCount = 1,
			.pSignalSemaphoreInfos = &drawSignal
		};
		std::vector<VkSubmitInfo2> submitInfos{ computeSubmit, drawSubmit };
		vkQueueSubmit2(Global::getDevices().getGraphicsQueues()[0], 2, submitInfos.data(), hitman.pOneAtATime);

		VkSemaphoreWaitInfo presentWait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			.semaphoreCount = 1,
			.pSemaphores = &hitman.pTimeline,
			.pValues = &drawSignalVal
		};
		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.swapchainCount = 1,
			.pSwapchains = &Global::getSwapchain().getSwapchain(),
			.pImageIndices = &nextImageIndex
		};
		vkWaitSemaphores(Global::getDevices().getLogicalDevice(), &presentWait, UINT64_MAX);

		if(vkQueuePresentKHR(Global::getDevices().getGraphicsQueues()[0], &presentInfo) == VK_ERROR_OUT_OF_DATE_KHR || Global::getWindow().framebufferResized) {
			windowResizeRecreate();
			return;
		}

		gHitmanIndex = (gHitmanIndex + 1) % gHitmenInFlight;
	}

	void renderLoop() {
		uint16_t nextSecondMark = 1;
		uint16_t accumulatedFramesCount = 0;

		while(!glfwWindowShouldClose(Global::getWindow().getGlfwWindow())) {
			glfwPollEvents();
			if(glfwGetKey(Global::getWindow().getGlfwWindow(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(Global::getWindow().getGlfwWindow(), GLFW_TRUE);
			}

			runNextSwapchainImage();
			
			accumulatedFramesCount++;
			if(glfwGetTime() > nextSecondMark) {
				nextSecondMark++;
				std::cout << "Frames last second: " << accumulatedFramesCount << '\n';
				accumulatedFramesCount = 0;
			}
		}

		vkDeviceWaitIdle(Global::getDevices().getLogicalDevice());
	}
}
