#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include "Swapchain.h"

namespace FrameData {
	struct SyncTargets {
		uint64_t wait_val{};
		uint64_t signal_val{};
	};

	struct SyncData {
		VkFence guard{};
		VkSemaphore timeline_semaphore{};
		uint64_t val{};
		std::vector<SyncTargets> targets{};

		void increment_targets() {
			for(SyncTargets& target : targets) {
				target.wait_val = val;
				target.signal_val = ++val;
			}
		}
	};

	struct SubmitData {
		VkSemaphoreSubmitInfo wait_info{};
		VkCommandBufferSubmitInfo cmd_info{};
		VkSemaphoreSubmitInfo signal_info{};
		VkSubmitInfo2 submit_info{};

		SubmitData(VkSemaphore wait_semaphore, VkSemaphore signal_semaphore) :
			wait_info{ .semaphore = wait }

		void setup() {
			submit_info.pWaitSemaphoreInfos = &wait_info;
			submit_info.pCommandBufferInfos = &cmd_info;
			submit_info.pSignalSemaphoreInfos = &signal_info;
		}

		void set_wait_semaphore(VkSemaphore semaphore) {
			wait_info.semaphore = semaphore;
		}

		void set_signal_semaphore(VkSemaphore semaphore) {
			signal_info.semaphore = semaphore;
		}
	};

	struct FrameData {
		SyncData sync_data{};
		std::vector<SubmitData> submit_datas{};

		explicit FrameData(const SyncData& sync_data, const std::vector<SubmitData>& submit_datas) :
			sync_data(sync_data), submit_datas(submit_datas) {
			selfRefer();
		}

		private:
		void selfRefer() {
			for(SubmitData& submit_data : submit_datas) {
				submit_data.setup();
				submit.wait_info.semaphore = sync_data.timeline_semaphore;
				submit.signal_info.semaphore = sync_data.timeline_semaphore;

				submit.submit_info.pWaitSemaphoreInfos = &submit.wait_info;
				submit.submit_info.pCommandBufferInfos = &submit.cmd_info;
				submit.submit_info.pSignalSemaphoreInfos = &submit.signal_info;
			}
		}
	};

	inline VkCommandPool gCmdPool{};
	inline std::vector<FrameData> gFrameData{};
	inline uint32_t g_frame_index = 0;
	inline constexpr uint32_t g_FRAMES_IN_FLIGHT = Swapchain::gIMAGE_COUNT;

	VkCommandPool createCmdPool(VkCommandPoolCreateFlags const&, uint32_t const&);
	VkCommandBuffer createCmdBuffer(VkCommandPool, VkCommandBufferLevel const&);
	VkFence createFence(VkFenceCreateFlags const&);
	VkSemaphore createSemaphore(VkSemaphoreTypeCreateInfo const&);

	void init();
	void deInit();
	void recreate();

	void createCmdPool();
	void createFrameData();

	void clearFrameData();
}
