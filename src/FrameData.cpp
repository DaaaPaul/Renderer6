#include "FrameData.h"
#include "Util.h"
#include "PhysicalDevice.h"

namespace Engine {
	namespace FrameData {
		void init() {
			createCmdPool();
			createFrameData();
		}

		void deInit() {
			clearFrameData();
			vkDestroyCommandPool(gDevice, gCmdPool, nullptr);
		}

		void recreate() {
			gFrameIndex = 0;
			clearFrameData();
			createFrameData();
		}

		void createCmdPool() {
			gCmdPool = Util::FrameData::createCmdPool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, Backend::PhysicalDevice::gQueueFamilyIndices[0]);
		}

		void createFrameData() {
			for(int i = 0; i < gFRAMES_IN_FLIGHT; ++i) {
				gFrameData.emplace_back(
					Util::FrameData::createFence(VK_NO_FLAGS),
					Util::FrameData::createSemaphore(VkSemaphoreTypeCreateInfo{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO, .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE, .initialValue = 0 }),
					0,
					std::vector<WaitSignal>{
						WaitSignal{0, 0},
						WaitSignal{0, 0},
						WaitSignal{0, 0}
					},
					std::vector<SubmitData>{
						SubmitData{
							VkSemaphoreSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
								.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT
							},
							VkCommandBufferSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
								.commandBuffer = Util::FrameData::createCmdBuffer(gCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY)
							},
							VkSemaphoreSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
								.stageMask = VK_PIPELINE_STAGE_2_COMPUTE_SHADER_BIT
							},
							VkSubmitInfo2{
								.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
								.waitSemaphoreInfoCount = 1,
								.commandBufferInfoCount = 1,
								.signalSemaphoreInfoCount = 1
							}
						},
						SubmitData{
							VkSemaphoreSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
								.stageMask = VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT
							},
							VkCommandBufferSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
								.commandBuffer = Util::FrameData::createCmdBuffer(gCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY)
							},
							VkSemaphoreSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
								.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
							},
							VkSubmitInfo2{
								.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
								.waitSemaphoreInfoCount = 1,
								.commandBufferInfoCount = 1,
								.signalSemaphoreInfoCount = 1
							}
						},
						SubmitData{
							VkSemaphoreSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
								.stageMask = VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT
							},
							VkCommandBufferSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
								.commandBuffer = Util::FrameData::createCmdBuffer(gCmdPool, VK_COMMAND_BUFFER_LEVEL_PRIMARY)
							},
							VkSemaphoreSubmitInfo{
								.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
								.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT
							},
							VkSubmitInfo2{
								.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
								.waitSemaphoreInfoCount = 1,
								.commandBufferInfoCount = 1,
								.signalSemaphoreInfoCount = 1
							}
						}
					}
				);
			}
		}

		void clearFrameData() {
			for(FrameData& frameData : gFrameData) {
				vkDestroyFence(gDevice, frameData.sync.guard, nullptr);
				vkDestroySemaphore(gDevice, frameData.sync.timeline, nullptr);

				for(SubmitData& submit : frameData.submits) {
					vkFreeCommandBuffers(gDevice, gCmdPool, 1, &submit.cmds.commandBuffer);
				}
			}
			gFrameData.clear();
		}
	}
}