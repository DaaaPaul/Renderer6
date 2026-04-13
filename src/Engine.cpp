#include <iostream>
#include <chrono>
#include "Engine.h"
#include "FrameData.h"
#include "LogicalDevice.h"
#include "PhysicalDevice.h"
#include "Swapchain.h"
#include "MemoryDevice.h"
#include "MemoryHost.h"
#include "Window.h"
#include "Pipelines.h"
#include "PipelineLayouts.h"
#include "Resources.h"
#include "ImageViewHotspot.h"

#define CHECK_PRESSED(glfwKey) \
	glfwGetKey(Backend::Window::gpGlfwWindow, glfwKey) == GLFW_PRESS

namespace Engine {
	void recordComputeCommands(VkCommandBuffer& pCommandBuffer) {
		vkResetCommandBuffer(pCommandBuffer, 0);

		constexpr VkCommandBufferBeginInfo BEGIN{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		CHECK_VK_SUCCESS(vkBeginCommandBuffer(pCommandBuffer, &BEGIN), "Failed to begin compute command buffer")
		
		vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Pipelines::gPipelines[2]);
			
		std::vector<VkDeviceAddress> pushConstantPointers{
			Memory::Host::gBuffers[11 + FrameData::gFrameIndex].address, // Delta time uniform buffer
			Memory::Host::gBuffers[2 + FrameData::gFrameIndex].address, // PARTICLES_IN SSBO
			Memory::Host::gBuffers[2 + ((FrameData::gFrameIndex + 1) % FrameData::gFRAMES_IN_FLIGHT)].address // particlesOut SSBO
		};
		vkCmdPushConstants(pCommandBuffer, PipelineLayouts::gLayouts[2], VK_SHADER_STAGE_COMPUTE_BIT, 0, POINTER_SIZE(3), pushConstantPointers.data());
		vkCmdDispatch(pCommandBuffer, Resources::gPARTICLES_COUNT / 256, 1, 1);

		CHECK_VK_SUCCESS(vkEndCommandBuffer(pCommandBuffer), "Failed to end compute command buffer")
	}

	void recordDrawModelCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX) {
		vkResetCommandBuffer(pCommandBuffer, 0);

		VkImageView pSwapchainImageView = ImageViewHotspot::newView(
			VkImageViewCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = Swapchain::gImages[IMAGE_INDEX],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = Swapchain::gIMAGE_FORMAT,
				.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
			}
		);
		VkImageView pDepthImageView = ImageViewHotspot::newView(
			VkImageViewCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = Memory::Device::gImages[1].image,
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = VK_FORMAT_D32_SFLOAT,
				.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1)
			}
		);
		
		VkRenderingAttachmentInfo colorAttachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = pSwapchainImageView,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearColorValue({0.4f, 0.2f, 0.2f, 1.0f}),
		};
		VkRenderingAttachmentInfo depthAttachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = pDepthImageView,
			.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
			.clearValue = { .depthStencil = VkClearDepthStencilValue(1.0f, 0) },
		};
		VkRenderingInfo renderingInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D(VkOffset2D(0, 0), Swapchain::gCurrentSwapchainStatus.imageExtent),
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachment,
			.pDepthAttachment = &depthAttachment
		};

		constexpr VkCommandBufferBeginInfo BEGIN{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		CHECK_VK_SUCCESS(vkBeginCommandBuffer(pCommandBuffer, &BEGIN), "Failed to begin command buffer")

		vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::gPipelines[0]);
		setViewportAndScissor(pCommandBuffer);

		constexpr VkDeviceSize ZERO = 0;
		vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, &Memory::Device::gBuffers[0].buffer, &ZERO);
		vkCmdBindIndexBuffer(pCommandBuffer, Memory::Device::gBuffers[1].buffer, 0, VK_INDEX_TYPE_UINT32);

		std::vector<VkDescriptorSet> drawDescriptorSets{
			Memory::Device::gDescriptorSets[0].set // texture image and sampler
		};
		vkCmdBindDescriptorSets(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayouts::gLayouts[0], 0, 1, drawDescriptorSets.data(), 0, nullptr);
		std::vector<VkDeviceAddress> pushConstantPointers{
			Memory::Host::gBuffers[3 + FrameData::gFrameIndex].address // address of transformation matrices
		};
		vkCmdPushConstants(pCommandBuffer, PipelineLayouts::gLayouts[0], VK_SHADER_STAGE_VERTEX_BIT, 0, POINTER_SIZE(1), pushConstantPointers.data()); 
		
		Util::Vulkan::transitionImageLayout(pCommandBuffer, Swapchain::gImages[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

		Util::Vulkan::transitionImageLayout(pCommandBuffer, Memory::Device::gImages[1].image, VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

		vkCmdBeginRendering(pCommandBuffer, &renderingInfo);
		vkCmdDrawIndexed(pCommandBuffer, Resources::gModelVertices.size(), 1, 0, 0, 0);
		vkCmdEndRendering(pCommandBuffer);

		CHECK_VK_SUCCESS(vkEndCommandBuffer(pCommandBuffer), "Command buffer end recording failure")

		ImageViewHotspot::pop();
		ImageViewHotspot::pop();
	};

	void recordDrawParticlesCommands(VkCommandBuffer& pCommandBuffer, uint32_t const& IMAGE_INDEX) {
		vkResetCommandBuffer(pCommandBuffer, 0);

		VkImageView pSwapchainImageView = ImageViewHotspot::newView(
			VkImageViewCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = Swapchain::gImages[IMAGE_INDEX],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = Swapchain::gIMAGE_FORMAT,
				.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
			}
		);
		VkRenderingAttachmentInfo colorAttachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = pSwapchainImageView,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
		};
		VkRenderingInfo renderingInfo{
			.sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
			.renderArea = VkRect2D(VkOffset2D(0, 0), Swapchain::gCurrentSwapchainStatus.imageExtent),
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &colorAttachment,
		};

		constexpr VkCommandBufferBeginInfo BEGIN{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		CHECK_VK_SUCCESS(vkBeginCommandBuffer(pCommandBuffer, &BEGIN), "Failed to begin command buffer")

		vkCmdBindPipeline(pCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::gPipelines[1]);
		setViewportAndScissor(pCommandBuffer);

		constexpr VkDeviceSize ZERO = 0;
		vkCmdBindVertexBuffers(pCommandBuffer, 0, 1, &Memory::Device::gBuffers[1 + 1 + ((FrameData::gFrameIndex + 1) % FrameData::gFRAMES_IN_FLIGHT)].buffer, &ZERO);

		vkCmdBeginRendering(pCommandBuffer, &renderingInfo);
		vkCmdDraw(pCommandBuffer, Resources::gPARTICLES_COUNT, 1, 0, 0);
		vkCmdEndRendering(pCommandBuffer);

		Util::Vulkan::transitionImageLayout(pCommandBuffer, Swapchain::gImages[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_ACCESS_2_NONE, 
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

		CHECK_VK_SUCCESS(vkEndCommandBuffer(pCommandBuffer), "Command buffer end recording failure")

		ImageViewHotspot::pop();
	}

	void renderNext() {
		FrameData::FrameData& currentFrame = FrameData::gFrameData[FrameData::gFrameIndex];
		uint64_t computeWaitVal = currentFrame.timelineVal;
		uint64_t computeSignalVal = ++currentFrame.timelineVal;
		uint64_t modelWaitVal = computeSignalVal;
		uint64_t modelSignalVal = ++currentFrame.timelineVal;
		uint64_t particlesWaitVal = modelSignalVal;
		uint64_t particlesSignalVal = ++currentFrame.timelineVal;

		uint32_t nextImageIndex = UINT32_MAX;
		if(vkAcquireNextImageKHR(gpDevice, Swapchain::gpSwapchain, UINT64_MAX, VK_NULL_HANDLE, currentFrame.oneAtATime, &nextImageIndex) == VK_ERROR_OUT_OF_DATE_KHR || Backend::Window::gFramebufferResized) {
			resize();
			return;
		}
		vkWaitForFences(gpDevice, 1, &currentFrame.oneAtATime, VK_TRUE, UINT64_MAX);
		vkResetFences(gpDevice, 1, &currentFrame.oneAtATime);

		// compute
		recordComputeCommands(currentFrame.computeCmds);
		updateDeltaTime();
		VkSemaphoreSubmitInfo computeWait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = currentFrame.timeline,
			.value = computeWaitVal,
			.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT,
		};
		VkCommandBufferSubmitInfo computeCommands{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = currentFrame.computeCmds,
		};
		VkSemaphoreSubmitInfo computeSignal{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = currentFrame.timeline,
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

		// model
		recordDrawModelCommands(currentFrame.modelCmds, nextImageIndex);
		updateTransformation();
		VkSemaphoreSubmitInfo modelWait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = currentFrame.timeline,
			.value = modelWaitVal,
			.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT,
		};
		VkCommandBufferSubmitInfo modelCommands{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = currentFrame.modelCmds,
		};
		VkSemaphoreSubmitInfo modelSignal{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = currentFrame.timeline,
			.value = modelSignalVal,
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		};
		VkSubmitInfo2 modelSubmit{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = 1,
			.pWaitSemaphoreInfos = &modelWait,
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &modelCommands,
			.signalSemaphoreInfoCount = 1,
			.pSignalSemaphoreInfos = &modelSignal
		};

		// particles
		recordDrawParticlesCommands(currentFrame.particleCmds, nextImageIndex);
		VkSemaphoreSubmitInfo particlesWait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = currentFrame.timeline,
			.value = particlesWaitVal,
			.stageMask = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT,
		};
		VkCommandBufferSubmitInfo particlesCommands{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
			.commandBuffer = currentFrame.particleCmds,
		};
		VkSemaphoreSubmitInfo particlesSignal{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
			.semaphore = currentFrame.timeline,
			.value = particlesSignalVal,
			.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
		};
		VkSubmitInfo2 particlesSubmit{
			.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
			.waitSemaphoreInfoCount = 1,
			.pWaitSemaphoreInfos = &particlesWait,
			.commandBufferInfoCount = 1,
			.pCommandBufferInfos = &particlesCommands,
			.signalSemaphoreInfoCount = 1,
			.pSignalSemaphoreInfos = &particlesSignal
		};

		std::vector<VkSubmitInfo2> submits{ computeSubmit, modelSubmit, particlesSubmit };
		vkQueueSubmit2(Backend::LogicalDevice::gQueues[0], 3, submits.data(), VK_NULL_HANDLE);

		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.swapchainCount = 1,
			.pSwapchains = &Swapchain::gpSwapchain,
			.pImageIndices = &nextImageIndex
		};
		VkSemaphoreWaitInfo presentWait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			.semaphoreCount = 1,
			.pSemaphores = &currentFrame.timeline,
			.pValues = &particlesSignalVal
		};
		vkWaitSemaphores(gpDevice, &presentWait, UINT64_MAX);

		if(vkQueuePresentKHR(Backend::LogicalDevice::gQueues[0], &presentInfo) == VK_ERROR_OUT_OF_DATE_KHR || Backend::Window::gFramebufferResized) {
			resize();
			return;
		}

		FrameData::gFrameIndex = (FrameData::gFrameIndex + 1) % FrameData::gFRAMES_IN_FLIGHT;
	}

	void run() {
		uint16_t nextSecondMark = 1;
		uint16_t accumulatedFramesCount = 0;

		while(!glfwWindowShouldClose(Backend::Window::gpGlfwWindow)) {
			glfwPollEvents();
			if(glfwGetKey(Backend::Window::gpGlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
				glfwSetWindowShouldClose(Backend::Window::gpGlfwWindow, GLFW_TRUE);
			}

			renderNext();
			
			accumulatedFramesCount++;
			if(glfwGetTime() > nextSecondMark) {
				nextSecondMark++;
				std::cout << "Frames last second: " << accumulatedFramesCount << '\n';
				accumulatedFramesCount = 0;
			}
		}

		vkDeviceWaitIdle(Backend::LogicalDevice::gpDevice);
	}

	void setViewportAndScissor(VkCommandBuffer& cmdBuffer) {
		VkViewport viewport{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(Swapchain::gCurrentSwapchainStatus.imageExtent.width),
			.height = static_cast<float>(Swapchain::gCurrentSwapchainStatus.imageExtent.height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
		VkRect2D scissor{
			.offset = VkOffset2D(0, 0),
			.extent = Swapchain::gCurrentSwapchainStatus.imageExtent
		};
		vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
		vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
	}

	void resize() {
		vkDeviceWaitIdle(gpDevice);

		Swapchain::recreate();
		// TODO: add functionality to recreate depth resources
		Backend::Window::gFramebufferResized = false;
		FrameData::recreate();
	}

	float getDeltaTime() noexcept {
		static std::chrono::steady_clock::time_point previousCallTime = std::chrono::high_resolution_clock::now();
		std::chrono::steady_clock::time_point nowTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(nowTime - previousCallTime).count();
		previousCallTime = nowTime;

		return deltaTime;
	}

	void updateTransformation() {
		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_X)) {
			gTransformation.model = glm::translate(gTransformation.model, glm::vec3(-0.001f, 0.0f, 0.0f));
		} else if(CHECK_PRESSED(GLFW_KEY_X)) {
			gTransformation.model = glm::translate(gTransformation.model, glm::vec3(0.001f, 0.0f, 0.0f));
		}
		
		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_Y)) {
			gTransformation.model = glm::translate(gTransformation.model, glm::vec3(0.0f, -0.001f, 0.0f));
		} else if(CHECK_PRESSED(GLFW_KEY_Y)) {
			gTransformation.model = glm::translate(gTransformation.model, glm::vec3(0.0f, 0.001f, 0.0f));
		}

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_Z)) {
			gTransformation.model = glm::translate(gTransformation.model, glm::vec3(0.0f, 0.0f, -0.001f));
		} else if(CHECK_PRESSED(GLFW_KEY_Z)) {
			gTransformation.model = glm::translate(gTransformation.model, glm::vec3(0.0f, 0.0f, 0.001f));
		}

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_S)) {
			gTransformation.model = glm::scale(gTransformation.model, glm::vec3(0.999f, 0.999f, 0.999f));
		} else if(CHECK_PRESSED(GLFW_KEY_S)) {
			gTransformation.model = glm::scale(gTransformation.model, glm::vec3(1.001f, 1.001f, 1.001f));
		}

		if(CHECK_PRESSED(GLFW_KEY_LEFT_SHIFT) && CHECK_PRESSED(GLFW_KEY_R)) {
			gTransformation.model = glm::rotate(gTransformation.model, glm::radians(-0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
		} else if(CHECK_PRESSED(GLFW_KEY_R)) {
			gTransformation.model = glm::rotate(gTransformation.model, glm::radians(0.1f), glm::vec3(1.0f, 0.0f, 0.0f));
		}

		Memory::Host::Mutate::writeToBuffer(3 + FrameData::gFrameIndex, &gTransformation, sizeof(gTransformation));
	}

	void updateDeltaTime() {
		float deltaTime = getDeltaTime();
		Memory::Host::Mutate::writeToBuffer(11 + FrameData::gFrameIndex, &deltaTime, sizeof(deltaTime));
	}
}
