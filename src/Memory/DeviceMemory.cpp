#include "DeviceMemory.hpp"

DeviceMemory::DeviceMemory(std::vector<Buffer>& buffers, std::vector<Image>& images) :
	Memory(buffers, images, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, nullptr) {

}