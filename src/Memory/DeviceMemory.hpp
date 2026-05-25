#pragma once

#include "Memory.hpp"

class DeviceMemory : public Memory {
	public:
	DeviceMemory() = default;
	explicit DeviceMemory(const std::vector<Buffer*>& p_buffers, const std::vector<Image*>& p_images);
};