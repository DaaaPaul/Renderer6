#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include <cassert>
#include "Swapchain.h"
#include "LogicalDevice.h"

namespace FrameKits {
	struct SyncKit {
		VkFence guard{};
		VkSemaphore timeline_semaphore{};
		uint64_t val{};

		explicit SyncKit(VkFence guard, VkSemaphore timeline_semaphore) :
			guard{ guard }, timeline_semaphore{ timeline_semaphore }, val{ 0 } {}

		void destroy() noexcept {
			vkDestroyFence(g_device, guard, nullptr);
			vkDestroySemaphore(g_device, timeline_semaphore, nullptr);
		}
	};

	inline VkCommandPool g_cmd_pool{};

	struct SubmitKit {
		VkSemaphoreSubmitInfo wait_info{};
		VkCommandBufferSubmitInfo cmd_info{};
		VkSemaphoreSubmitInfo signal_info{};
		VkSubmitInfo2 submit_info{};

		explicit SubmitKit(VkSemaphore semaphore, VkPipelineStageFlags2 sync_scope_1, VkPipelineStageFlags2 sync_scope_2, VkCommandBuffer cmd_buf) :
			wait_info{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, .semaphore = semaphore, .stageMask = sync_scope_1 },
			cmd_info{ .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO, .commandBuffer = cmd_buf },
			signal_info{ .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, .semaphore = semaphore, .stageMask = sync_scope_2 },
			submit_info{ .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2, .waitSemaphoreInfoCount = 1, .commandBufferInfoCount = 1, .signalSemaphoreInfoCount = 1 } {
			
			self_refer();
		}
	
		void self_refer() {
			submit_info.pWaitSemaphoreInfos = &wait_info;
			submit_info.pCommandBufferInfos = &cmd_info;
			submit_info.pSignalSemaphoreInfos = &signal_info;
		}

		void destroy() noexcept {
			vkFreeCommandBuffers(g_device, g_cmd_pool, 1, &cmd_info.commandBuffer);
		}
	};

	struct SyncPair {
		uint64_t wait_val{};
		uint64_t signal_val{};
	};

	struct FrameKit {
		SyncKit sync_kit;
		std::vector<SubmitKit> submit_kits;
		std::vector<SyncPair> sync_pairs{};

		void progress_sync() {
			assert(submit_kits.size() == sync_pairs.size());
			
			for(int i = 0; i < submit_kits.size(); ++i) {
				sync_pairs[i].wait_val = sync_kit.val;
				sync_pairs[i].signal_val = ++sync_kit.val;

				submit_kits[i].wait_info.value = sync_pairs[i].wait_val;
				submit_kits[i].signal_info.value = sync_pairs[i].signal_val;
			}
		}

		void destroy() noexcept {
			sync_kit.destroy();

			for(SubmitKit& submit_kit : submit_kits) {
				submit_kit.destroy();
			}
		}
	};

	inline std::vector<FrameKit> g_frame_kits{};
	inline uint32_t g_frame_index = 0;
	inline constexpr uint32_t g_FRAMES_IN_FLIGHT = Swapchain::g_IMAGE_COUNT;

	void init();
	void destroy();
	void recreate();

	VkCommandPool create_cmd_pool();
	std::vector<FrameKit> create_frame_kits();

	inline void increment_frame_index() {
		g_frame_index = (g_frame_index + 1) % g_FRAMES_IN_FLIGHT;
	}
}
