#include "FrameData.h"
#include "Util.h"
#include "PhysicalDevice.h"

namespace Engine {
	namespace FrameData {
		void init() {
			createCmdPool();
			createFrameData();
		}

		void deInit() noexcept {
			clearFrameData();
			vkDestroyCommandPool(gpDevice, gCmdPool, nullptr);
		}

		void recreate() {
			clearFrameData();
			createFrameData();
		}

		void createCmdPool() {
			gCmdPool = Util::FrameData::createCmdPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, Backend::PhysicalDevice::gQueueFamilyIndices[0]);
		}

		void createFrameData() {
			for(int i = 0; i < gFRAMES_IN_FLIGHT; i++) {
				gFrameData.emplace_back(
					Util::FrameData::createFence(0),
					Util::FrameData::createSemaphore(VkSemaphoreTypeCreateInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO, .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE, .initialValue = 0 }),
					0,
					Util::FrameData::createCmdBuffer(gCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY),
					Util::FrameData::createCmdBuffer(gCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY),
					Util::FrameData::createCmdBuffer(gCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY)
				);
			}
		}

		void clearFrameData() noexcept {
			for(FrameData& frameData : gFrameData) {
				vkDestroyFence(gpDevice, frameData.oneAtATime, nullptr);
				vkDestroySemaphore(gpDevice, frameData.timeline, nullptr);
				vkFreeCommandBuffers(gpDevice, gCmdPool, 1, &frameData.modelCmds);
				vkFreeCommandBuffers(gpDevice, gCmdPool, 1, &frameData.computeCmds);
				vkFreeCommandBuffers(gpDevice, gCmdPool, 1, &frameData.particleCmds);
			}
			gFrameData.clear();
		}
	}
}