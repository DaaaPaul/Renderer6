#include "DeviceMemory.hpp"

DeviceMemory::DeviceMemory(std::vector<Buffer>& buffers, std::vector<Image>& images) :
	Memory(buffers, images, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &memory_allocate_address),
	buffer_addresses(get_buffer_addresses(Buffer::get_vk_buffers(p_buffers))) {

}

std::vector<VkDeviceAddress> DeviceMemory::get_buffer_addresses(const std::vector<VkBuffer>& buffers) {
	std::vector<VkDeviceAddress> buffer_addresses(buffers.size());
		
	VkBufferDeviceAddressInfo buffer_address_info{
		.sType = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO,
		.buffer = VK_NULL_HANDLE
	};
	for(int i = 0; i < buffers.size(); ++i) {
		buffer_address_info.buffer = buffers[i];
			
		buffer_addresses[i] = vkGetBufferDeviceAddress(g_device, &buffer_address_info);
	}

	return buffer_addresses;
}