#pragma once

#include "Memory.hpp"

class HostMemory : public Memory {
	private:
	std::vector<void*> buffer_maps{};

	static constexpr VkMemoryAllocateFlagsInfo memory_allocate_address{
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO,
		.flags = VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT
	};

	public:
	HostMemory() = default;

	explicit HostMemory(std::vector<Buffer>& buffers);
	void destroy() noexcept override;

	static VkDeviceAddress get_buffer_address(VkBuffer buffer);

	void* get_buffer_map(const Buffer& buffer) const;

	private:
	static std::vector<void*> map_buffers(VkDeviceMemory memory, 
										  const std::vector<VkDeviceSize>& buffer_offsets, 
										  const std::vector<VkDeviceSize>& buffer_sizes,
										  const std::vector<VkBuffer>& buffers);
};