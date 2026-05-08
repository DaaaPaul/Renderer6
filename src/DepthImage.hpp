#pragma once

#include <vulkan/vulkan.h>

class DepthImage {
	private:
	VkImage image{};
	VkImageView image_view{};

	VkMemoryRequirements memory_requirements{};

	public:
	explicit DepthImage(VkExtent3D);
	~DepthImage();

	VkImage get_image() const { return image; }
	VkImageView get_image_view() const { return image_view; }
	VkMemoryRequirements get_memory_requirements() const { return memory_requirements; }

	private:
	static VkImage create_image(VkExtent3D extent);
	static VkImageView create_image_view(VkImage image);
};
