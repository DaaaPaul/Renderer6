#pragma once

#include "Memory.hpp"

class HostMemory : public Memory {
	private:
	static constexpr VkMemoryAllocateFlagsInfo memory_allocate_address{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
		.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
	};

	public:
	HostMemory() = default;

	explicit HostMemory(const std::vector<Buffer*>& buffers);
	void destroy() noexcept override;

	static VkDeviceAddress get_buffer_address(VkBuffer buffer);
	void copy_data_to_buffer(const Buffer* buffer, const void* src, VkDeviceSize size);
};