#include "DeviceMemory.hpp"

DeviceMemory::DeviceMemory(const std::vector<Buffer*>& p_buffers, const std::vector<Image*>& p_images) :
	Memory(p_buffers, p_images, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr) {

}