#pragma once

#include "Memory.hpp"

class DeviceMemory : public Memory {
	public:
	DeviceMemory() = default;
	explicit DeviceMemory(std::vector<Buffer>& buffers, std::vector<Image>& images);
};