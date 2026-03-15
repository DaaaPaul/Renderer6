#include "Frames.hpp"
#include "Common.h"
#include "Global.h"

namespace Engine {
	Frames::Frame::Frame(VkCommandPool pPool) :
	pOneAtATime{},
	pTimeline{},
	timelineVal{ 0 },
	pPoolUsed{ pPool },
	pModelCommands{},
	pComputeCommands{},
	pParticleCommands{} {
		VkFenceCreateInfo signedFenceCreate{
			.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
			/*.flags = VK_FENCE_CREATE_SIGNALED_BIT*/
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
			.pNext = &timelineType
		};
		CHECK_VK_SUCCESS(
			vkCreateSemaphore(Global::getDevices().getLogicalDevice(), &timelineSemaphoreInfo, nullptr, &pTimeline),
			"Semaphore creation failed"
		)

		VkCommandBufferAllocateInfo commandBufferInfo{
			.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
			.commandPool = pPoolUsed,
			.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
			.commandBufferCount = 1,
		};
		CHECK_VK_SUCCESS(
			vkAllocateCommandBuffers(Global::getDevices().getLogicalDevice(), &commandBufferInfo, &pModelCommands),
			"Command buffer creation failed"
		)
		CHECK_VK_SUCCESS(
			vkAllocateCommandBuffers(Global::getDevices().getLogicalDevice(), &commandBufferInfo, &pComputeCommands),
			"Command buffer creation failed"
		)
		CHECK_VK_SUCCESS(
			vkAllocateCommandBuffers(Global::getDevices().getLogicalDevice(), &commandBufferInfo, &pParticleCommands),
			"Command buffer creation failed"
		)
	}

	Frames::Frame::~Frame() {
		vkDestroyFence(Global::getDevices().getLogicalDevice(), pOneAtATime, nullptr);
		vkDestroySemaphore(Global::getDevices().getLogicalDevice(), pTimeline, nullptr);
		vkFreeCommandBuffers(Global::getDevices().getLogicalDevice(), pPoolUsed, 1, &pModelCommands);
		vkFreeCommandBuffers(Global::getDevices().getLogicalDevice(), pPoolUsed, 1, &pComputeCommands);
		vkFreeCommandBuffers(Global::getDevices().getLogicalDevice(), pPoolUsed, 1, &pParticleCommands);
	}

	Frames::Frames(uint16_t const& FRAMES_COUNT, uint32_t const& GRAPHICS_QF_INDEX) : 
		pCommandPool{},
		frames{} {
		VkCommandPoolCreateInfo poolCreate{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
			.queueFamilyIndex = GRAPHICS_QF_INDEX
		};

		CHECK_VK_SUCCESS(
			vkCreateCommandPool(Global::getDevices().getLogicalDevice(), &poolCreate, nullptr, &pCommandPool),
			"Failed to create command pool"
		)

		frames.reserve(FRAMES_COUNT);
		for(int i = 0; i < FRAMES_COUNT; i++) {
			frames.emplace_back(pCommandPool);
		}
	}

	Frames::~Frames() {
		frames.clear();
		vkDestroyCommandPool(Global::getDevices().getLogicalDevice(), pCommandPool, nullptr);
	}

	void Frames::recreateFrames() {
		size_t previousFrameCount = frames.size();

		frames.clear();
		frames.reserve(previousFrameCount);
		for(int i = 0; i < previousFrameCount; i++) {
			frames.emplace_back(pCommandPool);
		}
	}
}