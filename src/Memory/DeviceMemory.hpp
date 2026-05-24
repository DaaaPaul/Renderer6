#pragma once

#include "Memory.hpp"

class DeviceMemory : public Memory {
	private:
	std::vector<VkDeviceAddress> buffer_addresses{};

	static constexpr VkMemoryAllocateFlagsInfo memory_allocate_address{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
		.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
	};

	public:
	DeviceMemory() = default;
	explicit DeviceMemory(std::vector<Buffer>& buffers, std::vector<Image>& images);

	private:
	static std::vector<VkDeviceAddress> get_buffer_addresses(const std::vector<VkBuffer>& buffers);
};