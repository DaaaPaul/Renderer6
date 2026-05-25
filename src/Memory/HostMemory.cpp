#include "HostMemory.hpp"

HostMemory::HostMemory(std::vector<Buffer>& buffers) :
	Memory(buffers, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memory_allocate_address),
	buffer_maps(map_buffers(memory, properties.buffer_offsets, Buffer::get_buffer_sizes(p_buffers), Buffer::get_vk_buffers(p_buffers))) {

}

void HostMemory::destroy() noexcept {
	vkUnmapMemory(g_device, memory);
	vkFreeMemory(g_device, memory, nullptr);
}

std::vector<void*> HostMemory::map_buffers(VkDeviceMemory memory, 
	const std::vector<VkDeviceSize>& buffer_offsets, 
	const std::vector<VkDeviceSize>& buffer_sizes,
	const std::vector<VkBuffer>& buffers) {

	assert(buffer_offsets.size() == buffer_sizes.size() && buffer_sizes.size() == buffers.size());

	std::vector<void*> maps(buffers.size());
		
	for(int i = 0; i < buffers.size(); ++i) {
		VK_CHECK(vkMapMemory(g_device, memory, buffer_offsets[i], buffer_sizes[i], VK_NO_FLAGS, &maps[i]), "map_buffers: failed")
	}

	return maps;
}

VkDeviceAddress HostMemory::get_buffer_address(VkBuffer buffer) {
	VkDeviceAddress address = UINT64_MAX;

	VkBufferDeviceAddressInfo buffer_address_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
		.buffer = buffer
	};
	address = vkGetBufferDeviceAddress(g_device, &buffer_address_info);

	return address;
}

void* HostMemory::get_buffer_map(const Buffer& buffer) const {
	void* map = nullptr;

	for(int i = 0; i < p_buffers.size() && !map; ++i) {
		if(p_buffers[i] == &buffer) {
			map = buffer_maps[i];
		}
	}

	return map;
}