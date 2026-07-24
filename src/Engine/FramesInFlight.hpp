#pragma once

#include "Frame.hpp"

struct FramesInFlight {
	struct Index {
		uint32_t frames_in_flight{};
		uint32_t val{};

		explicit Index(uint32_t frames_in_flight);
		Index operator++() {
			val = (val + 1) % frames_in_flight;
			return *this;
		}
	};

	VkCommandPool cmd_pool{};
	Index frame_index;
	std::vector<FrameArgs> args;
	std::vector<Frame> frames;

	explicit FramesInFlight(uint32_t frames_in_flight, uint32_t frame_submit_count);

	~FramesInFlight();

	DELETE_COPYING_MOVING(FramesInFlight)
};