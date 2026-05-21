#pragma once

#include "Memory.hpp"

class DeviceMemory : public Memory {
	private:
	std::vector<VkDeviceAddress> buffer_addresses{};


};