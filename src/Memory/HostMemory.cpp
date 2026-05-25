#include "HostMemory.hpp"

HostMemory::HostMemory(const std::vector<Buffer*>& p_buffers) :
	Memory(p_buffers, {}, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &memory_allocate_address) {

}

void HostMemory::destroy() noexcept {
	vkUnmapMemory(g_device, memory);
	vkFreeMemory(g_device, memory, nullptr);
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

void HostMemory::copy_data_to_buffer(const Buffer* p_buffer, const void* src, VkDeviceSize size) {
	void* p_buffer_data = nullptr;
	size_t buffer_index = UINT64_MAX;

	for(int i = 0; i < p_buffers.size() && buffer_index == UINT64_MAX; ++i) {
		if(p_buffers[i] == p_buffer) {
			buffer_index = i;
		}
	}

	assert(buffer_index != UINT64_MAX);

	vkMapMemory(g_device, memory, properties.buffer_offsets[buffer_index], size, VK_NO_FLAGS, &p_buffer_data);
	
	std::memcpy(p_buffer_data, src, size);

	vkUnmapMemory(g_device, memory);
}