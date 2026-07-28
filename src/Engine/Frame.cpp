#include "engine/Frame.hpp"
#include "utility/Vulkan.h"
#include "utility/Utility.h"
#include "backend/PhysicalDevice.h"

Submit::Submit(VkCommandPool cmd_pool, VkSemaphore timeline) :
	cmd_pool{ cmd_pool },
	timeline{ timeline },
	wait{ 
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, 
		.semaphore = timeline 
	},
	cmd{ 
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO, 
		.commandBuffer = Vulkan::create_cmd_buffer(cmd_pool) 
	},
	signal{ 
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO, 
		.semaphore = timeline 
	},
	submit{ 
		.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2, 
		.waitSemaphoreInfoCount = 1, 
		.pWaitSemaphoreInfos = &wait,
		.commandBufferInfoCount = 1, 
		.pCommandBufferInfos = &cmd,
		.signalSemaphoreInfoCount = 1,
		.pSignalSemaphoreInfos = &signal
	} {
}

Submit::~Submit() {
	vkFreeCommandBuffers(g_device, cmd_pool, 1, &cmd.commandBuffer);
}

Frame::Frame(VkCommandPool cmd_pool, uint32_t submit_count) :
	cmd_pool{ cmd_pool },
	fence{ Vulkan::create_fence(Vulkan::NO_FLAGS) },
	timeline{ Vulkan::create_semaphore(VK_SEMAPHORE_TYPE_TIMELINE) },
	timeline_val{ 0 },
	args(submit_count, SubmitArgs{cmd_pool, timeline}),
	submits(args.begin(), args.end()) {

}

void Frame::progress_timeline() {
	for(Submit& submit : submits) {
		submit.wait.value = timeline_val;
		submit.signal.value = ++timeline_val;
	}
}

Frame::~Frame() {
	submits.clear();

	vkDestroySemaphore(g_device, timeline, nullptr);
	vkDestroyFence(g_device, fence, nullptr);
}