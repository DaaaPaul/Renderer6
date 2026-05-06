#pragma once

#include <vulkan/vulkan.h>

class DepthImage {
	private:
	VkImage image{};
	VkImageView imageView{};

	VkMemoryRequirements requirements{};

	public:
	explicit DepthImage(VkExtent3D);
	~DepthImage();

	VkImage getImage() const { return image; }
	VkImageView getImageView() const { return imageView; }
	VkMemoryRequirements getRequirements() const { return requirements; }

	private:
	static VkImage createImage(VkExtent3D);
	static VkImageView createImageView(VkImage);
};
