#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>
#include <iostream>
#include <vector>
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
#include "Util.h"
#include "Transforms.hpp"
#include "Camera.hpp"

namespace Engine {
	void recordComputeCommands(VkCommandBuffer cmdBuffer) {
		beginCmdBuffer(cmdBuffer);
		
		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, Pipelines::gPipelines[2]);

		std::vector<VkDeviceAddress> pushConstant{
			Memory::Host::gBuffers[3 + 2 * Swapchain::gIMAGE_COUNT + FrameData::gFrameIndex].address, // deltaTime
			Memory::Device::gBuffers[2 + FrameData::gFrameIndex].address, // particlesIn
			Memory::Device::gBuffers[2 + ((FrameData::gFrameIndex + 1) % FrameData::gFRAMES_IN_FLIGHT)].address // particlesOut
		};
		vkCmdPushConstants(cmdBuffer, PipelineLayouts::gLayouts[2], VK_SHADER_STAGE_COMPUTE_BIT, 0, POINTER_SIZE(3), pushConstant.data());
		vkCmdDispatch(cmdBuffer, Resources::gPARTICLES_COUNT / 256, 1, 1);

		CHECK_VK_SUCCESS(vkEndCommandBuffer(cmdBuffer), "Failed to end compute command buffer")
	}

	void recordDrawModelCommands(VkCommandBuffer cmdBuffer, uint32_t const& IMAGE_INDEX) {
		vkResetCommandBuffer(cmdBuffer, 0);

		VkImageView imageView = ImageViewHotspot::newView(
			VkImageViewCreateInfo{
				.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
				.image = Swapchain::gImages[IMAGE_INDEX],
				.viewType = VK_IMAGE_VIEW_TYPE_2D,
				.format = Swapchain::gIMAGE_FORMAT,
				.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1)
			}
		);
		VkImageView depthImageView = ImageViewHotspot::newView(
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
			.imageView = imageView,
			.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
			.resolveMode = VK_RESOLVE_MODE_NONE,
			.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
			.storeOp = VK_ATTACHMENT_STORE_OP_STORE,
			.clearValue = VkClearColorValue({0.4f, 0.2f, 0.2f, 1.0f}),
		};
		VkRenderingAttachmentInfo depthAttachment{
			.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
			.imageView = depthImageView,
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
		CHECK_VK_SUCCESS(vkBeginCommandBuffer(cmdBuffer, &BEGIN), "Failed to begin command buffer")

		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::gPipelines[0]);
		setViewportScissor(cmdBuffer);

		constexpr VkDeviceSize ZERO = 0;
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &Memory::Device::gBuffers[0].buffer, &ZERO);
		vkCmdBindIndexBuffer(cmdBuffer, Memory::Device::gBuffers[1].buffer, 0, VK_INDEX_TYPE_UINT32);

		std::vector<VkDescriptorSet> drawDescriptorSets{
			Memory::Device::gDescriptorSets[0].set // texture image and sampler
		};
		vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, PipelineLayouts::gLayouts[0], 0, 1, drawDescriptorSets.data(), 0, nullptr);
		std::vector<VkDeviceAddress> pushConstant{
			Memory::Host::gBuffers[3 + FrameData::gFrameIndex].address // address of transformation matrices
		};
		vkCmdPushConstants(cmdBuffer, PipelineLayouts::gLayouts[0], VK_SHADER_STAGE_VERTEX_BIT, 0, POINTER_SIZE(1), pushConstant.data()); 
		
		Util::Vulkan::transitionImageLayout(cmdBuffer, Swapchain::gImages[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

		Util::Vulkan::transitionImageLayout(cmdBuffer, Memory::Device::gImages[1].image, VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT, VK_ACCESS_2_NONE,
		VK_PIPELINE_STAGE_2_EARLY_FRAGMENT_TESTS_BIT, VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT, 
		VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

		vkCmdBeginRendering(cmdBuffer, &renderingInfo);
		vkCmdDrawIndexed(cmdBuffer, Resources::gModelIndices.size(), 1, 0, 0, 0);
		vkCmdEndRendering(cmdBuffer);

		CHECK_VK_SUCCESS(vkEndCommandBuffer(cmdBuffer), "Command buffer end recording failure")
	};

	void recordDrawParticlesCommands(VkCommandBuffer cmdBuffer, uint32_t const& IMAGE_INDEX) {
		vkResetCommandBuffer(cmdBuffer, 0);

		VkImageView imageView = ImageViewHotspot::newView(
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
			.imageView = imageView,
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
		CHECK_VK_SUCCESS(vkBeginCommandBuffer(cmdBuffer, &BEGIN), "Failed to begin command buffer")

		vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, Pipelines::gPipelines[1]);
		setViewportScissor(cmdBuffer);

		constexpr VkDeviceSize ZERO = 0;
		vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &Memory::Device::gBuffers[2 + ((FrameData::gFrameIndex + 1) % FrameData::gFRAMES_IN_FLIGHT)].buffer, &ZERO);

		vkCmdBeginRendering(cmdBuffer, &renderingInfo);
		vkCmdDraw(cmdBuffer, Resources::gPARTICLES_COUNT, 1, 0, 0);
		vkCmdEndRendering(cmdBuffer);

		Util::Vulkan::transitionImageLayout(cmdBuffer, Swapchain::gImages[IMAGE_INDEX], VkImageSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1), 
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
		VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT, VK_ACCESS_2_NONE, 
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, Backend::PhysicalDevice::gQueueFamilyIndices[0]);

		CHECK_VK_SUCCESS(vkEndCommandBuffer(cmdBuffer), "Command buffer end recording failure")
	}

	void renderNext() {
		FrameData::FrameData& frameData = FrameData::gFrameData[FrameData::gFrameIndex];
		frameData.sync.updateWaitSignals();

		uint32_t swapchainImageIndex = UINT32_MAX;
		if(acquireSwapchainImage(swapchainImageIndex, frameData.sync.guard)) {
			resize();
			return;
		}
		waitForFence(frameData.sync.guard);

		recordComputeCommands(frameData.submits[0].cmds.commandBuffer);
		frameData.submits[0].wait.value = frameData.sync.waitSignals[0].waitVal;
		frameData.submits[0].signal.value = frameData.sync.waitSignals[0].signalVal;

		recordDrawModelCommands(frameData.submits[1].cmds.commandBuffer, swapchainImageIndex);
		frameData.submits[1].wait.value = frameData.sync.waitSignals[1].waitVal;
		frameData.submits[1].signal.value = frameData.sync.waitSignals[1].signalVal;

		recordDrawParticlesCommands(frameData.submits[2].cmds.commandBuffer, swapchainImageIndex);
		frameData.submits[2].wait.value = frameData.sync.waitSignals[2].waitVal;
		frameData.submits[2].signal.value = frameData.sync.waitSignals[2].signalVal;

		std::vector<VkSubmitInfo2> submits{ frameData.submits[0].info, frameData.submits[1].info, frameData.submits[2].info };
		vkQueueSubmit2(Backend::LogicalDevice::gQueues[0], 3, submits.data(), VK_NULL_HANDLE);

		waitForTimelineSemaphore(frameData.sync.timeline, frameData.sync.waitSignals[2].signalVal);
		
		if(presentSwapchainImage(swapchainImageIndex, Backend::LogicalDevice::gQueues[0])) {
			resize();
			return;
		}

		FrameData::gFrameIndex = (FrameData::gFrameIndex + 1) % FrameData::gFRAMES_IN_FLIGHT;
	}

	void run() {
		std::chrono::steady_clock::time_point before{};

		while(!glfwWindowShouldClose(Backend::Window::gGlfwWindow)) {
			glfwPollEvents();
			update();

			before = std::chrono::high_resolution_clock::now();
			renderNext();
			std::cout << writeDeltaTime(std::chrono::high_resolution_clock::now(), before) << "s\n";
		}

		CHECK_VK_SUCCESS(vkDeviceWaitIdle(Backend::LogicalDevice::gDevice), "Failed to wait idle");
	}

	bool acquireSwapchainImage(uint32_t& index, VkFence fenceToSignal) {
		return vkAcquireNextImageKHR(gDevice, Swapchain::gSwapchain, UINT64_MAX, VK_NULL_HANDLE, fenceToSignal, &index) == VK_ERROR_OUT_OF_DATE_KHR || Backend::Window::gFramebufferResized;
	}

	bool presentSwapchainImage(uint32_t const& INDEX, VkQueue queue) {
		VkPresentInfoKHR presentInfo{
			.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
			.swapchainCount = 1,
			.pSwapchains = &Swapchain::gSwapchain,
			.pImageIndices = &INDEX
		};

		return vkQueuePresentKHR(queue, &presentInfo) == VK_ERROR_OUT_OF_DATE_KHR || Backend::Window::gFramebufferResized;
	}

	void waitForTimelineSemaphore(VkSemaphore timeline, uint64_t const& WAIT_VAL) {
		VkSemaphoreWaitInfo wait{
			.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
			.semaphoreCount = 1,
			.pSemaphores = &timeline,
			.pValues = &WAIT_VAL
		};
		CHECK_VK_SUCCESS(vkWaitSemaphores(gDevice, &wait, UINT64_MAX), "Failed to wait for semaphore");
	}

	void waitForFence(VkFence fence) {
		CHECK_VK_SUCCESS(vkWaitForFences(gDevice, 1, &fence, VK_TRUE, UINT64_MAX), "Failed to wait for fence");
		CHECK_VK_SUCCESS(vkResetFences(gDevice, 1, &fence), "Failed to reset fence");
	}

	void beginCmdBuffer(VkCommandBuffer cmdBuffer) {
		vkResetCommandBuffer(cmdBuffer, 0);
		constexpr VkCommandBufferBeginInfo BEGIN{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		CHECK_VK_SUCCESS(vkBeginCommandBuffer(cmdBuffer, &BEGIN), "Failed to begin compute command buffer")
	}

	void setViewportScissor(VkCommandBuffer cmdBuffer) {
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
		vkDeviceWaitIdle(gDevice);

		Swapchain::recreate();
		// TODO: add functionality to recreate depth resources
		Backend::Window::gFramebufferResized = false;
		FrameData::recreate();
	}

	void update() {
		if(glfwGetKey(Backend::Window::gGlfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(Backend::Window::gGlfwWindow, GLFW_TRUE);
		} else {
			gCamera.update();
			Vertex::Transforms transformation(glm::mat4(1.0f), convertViewMatrix(gCamera), convertProjMatrix(gCamera));

			Memory::Host::Mutate::writeToBuffer(3 + FrameData::gFrameIndex, &transformation, sizeof(transformation));
		}
	}

	float writeDeltaTime(std::chrono::steady_clock::time_point const& A, std::chrono::steady_clock::time_point const& B) {
		float deltaTime = std::chrono::duration<float, std::chrono::seconds::period>(A - B).count();
		Memory::Host::Mutate::writeToBuffer(3 + 2 * Swapchain::gIMAGE_COUNT + FrameData::gFrameIndex, &deltaTime, sizeof(deltaTime));
		
		return deltaTime;
	}
}
