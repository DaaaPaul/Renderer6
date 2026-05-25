#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <memory>
#include "Backend/LogicalDevice.h"

class Buffer {
	private:
	VkBuffer buffer{};
	uint32_t name_index{};
	
	public:
	virtual ~Buffer() = default;
	explicit Buffer(uint32_t name_index,
					VkBufferCreateFlags create_flags, 
					VkDeviceSize size,
					VkBufferUsageFlags usage_flags, 
					VkSharingMode sharing_mode,
					const std::vector<uint32_t>& queue_family_indices);
	virtual void destroy() noexcept;

	VkBuffer get_buffer() const { 
		return buffer; 
	}

	uint32_t get_name_index() const { 
		return name_index; 
	}

	VkMemoryRequirements get_memory_requirements() const { 
		VkMemoryRequirements memory_requirements{};
		vkGetBufferMemoryRequirements(g_device, buffer, &memory_requirements);

		return memory_requirements; 
	}

	static void copy_buffer(const Buffer* src, Buffer* dst, VkBufferCopy region);

	static std::vector<VkBuffer> get_vk_buffers(const std::vector<Buffer*>& p_buffers) {
		std::vector<VkBuffer> vk_buffers(p_buffers.size());

		for(int i = 0; i < p_buffers.size(); ++i) {
			vk_buffers[i] = p_buffers[i]->get_buffer();
		}

		return vk_buffers;
	}

	static std::vector<VkDeviceSize> get_buffer_sizes(const std::vector<Buffer*>& p_buffers) {
		std::vector<VkDeviceSize> buffer_sizes(p_buffers.size());

		for(int i = 0; i < p_buffers.size(); ++i) {
			buffer_sizes[i] = p_buffers[i]->get_memory_requirements().size;
		}

		return buffer_sizes;
	}
};

class Buffers {
	private:
	std::unordered_map<uint32_t, std::unique_ptr<Buffer>> buffer_map{};

	public:
	template<class BufferType, class... Args>
	BufferType* add(uint32_t name_index, Args&&... args) {
		static_assert(std::is_base_of<Buffer, BufferType>::value, "BufferType needs to inherit Buffer");
		BufferType* p_buffer = nullptr;

		auto i = buffer_map.find(name_index);

		if(i != buffer_map.end()) {
			p_buffer = static_cast<BufferType*>(i->second.get());
		} else {
			buffer_map[name_index] = std::make_unique<BufferType>(name_index, std::forward<Args>(args)...);
			p_buffer = static_cast<BufferType*>(buffer_map.at(name_index).get());
		}

		return p_buffer;
	}

	template<class BufferType>
	BufferType* get(uint32_t name_index) {
		static_assert(std::is_base_of<Buffer, BufferType>::value, "BufferType needs to inherit Buffer");

		BufferType* p_buffer = nullptr;

		auto i = buffer_map.find(name_index);

		if(i != buffer_map.end()) {
			p_buffer = static_cast<BufferType*>(i->second.get());
		}

		return p_buffer;
	}

	bool remove(uint32_t name_index) {
		bool remove_success = false;
		auto i = buffer_map.find(name_index);

		if(i != buffer_map.end()) {
			buffer_map.erase(i);
			remove_success = true;
		}

		return remove_success;
	}
};