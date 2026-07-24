#include "FramesInFlight.hpp"
#include "Utility/Vulkan.h"
#include "Backend/PhysicalDevice.h"

FramesInFlight::Index::Index(uint32_t frames_in_flight) :
	frames_in_flight{ frames_in_flight },
	val{ 0 } {
		
}

FramesInFlight::FramesInFlight(uint32_t frames_in_flight, uint32_t frame_submit_count) :
	cmd_pool{ Vulkan::create_cmd_pool(VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT)) },
	frame_index(frames_in_flight),
	args(frames_in_flight, FrameArgs{cmd_pool, frame_submit_count}),
	frames(args.begin(), args.end()) {

}

FramesInFlight::~FramesInFlight() {
	frames.clear();

	vkDestroyCommandPool(g_device, cmd_pool, nullptr);
}