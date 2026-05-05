#pragma once

#include <vector>
#include <vulkan/vulkan_core.h>
#include <cstdint>
#include "Swapchain.h"

namespace FrameData {
	struct WaitSignal {
		uint64_t waitVal{};
		uint64_t signalVal{};
	};

	struct SyncData {
		VkFence guard{};
		VkSemaphore timeline{};
		uint64_t timelineVal{};
		std::vector<WaitSignal> waitSignals{};

		void updateWaitSignals() {
			for(WaitSignal& waitSignal : waitSignals) {
				waitSignal.waitVal = timelineVal;
				waitSignal.signalVal = ++timelineVal;
			}
		}
	};

	struct SubmitData {
		VkSemaphoreSubmitInfo wait{};
		VkCommandBufferSubmitInfo cmds{};
		VkSemaphoreSubmitInfo signal{};
		VkSubmitInfo2 info{};
	};

	struct FrameData {
		SyncData sync{};
		std::vector<SubmitData> submits{};

		explicit FrameData(VkFence guard, VkSemaphore timeline, uint64_t const& TIMELINE_VAL, std::vector<WaitSignal> const& WAIT_SIGNALS, std::vector<SubmitData> const& SUBMITS) :
			sync{guard, timeline, TIMELINE_VAL, WAIT_SIGNALS}, submits(SUBMITS) {
			selfRefer();
		}

		private:
		void selfRefer() {
			for(SubmitData& submit : submits) {
				submit.wait.semaphore = sync.timeline;
				submit.signal.semaphore = sync.timeline;

				submit.info.pWaitSemaphoreInfos = &submit.wait;
				submit.info.pCommandBufferInfos = &submit.cmds;
				submit.info.pSignalSemaphoreInfos = &submit.signal;
			}
		}
	};

	inline VkCommandPool gCmdPool{};
	inline std::vector<FrameData> gFrameData{};
	inline uint32_t gFrameIndex = 0;
	inline constexpr uint32_t gFRAMES_IN_FLIGHT = Swapchain::gIMAGE_COUNT;

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
