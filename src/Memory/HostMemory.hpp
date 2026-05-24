#pragma once

#include "Memory.hpp"

class HostMemory : public Memory {
	private:
	std::vector<void*> buffer_maps{};

	public:
	HostMemory() = default;
	explicit HostMemory(std::vector<Buffer>& buffers);

	private:
	static std::vector<void*> map_buffers(VkDeviceMemory memory, 
										  const std::vector<VkDeviceSize>& buffer_offsets, 
										  const std::vector<VkDeviceSize>& buffer_sizes,
										  const std::vector<VkBuffer>& buffers);
};