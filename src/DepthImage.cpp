#include "DepthImage.hpp"
#include "Util.h"
#include "PhysicalDevice.h"

DepthImage::DepthImage(VkExtent3D extent) :
	image{ createImage(extent) }, imageView{ createImageView(image) } {
	vkGetImageMemoryRequirements(gDevice, image, &requirements);
}

DepthImage::~DepthImage() {
	vkDestroyImageView(gDevice, imageView, nullptr);
	vkDestroyImage(gDevice, image, nullptr);
}

VkImage DepthImage::createImage(VkExtent3D extent) {
	VkImage image{};

	VkImageCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.format = VK_FORMAT_D32_SFLOAT,
		.extent = extent,
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = VK_SAMPLE_COUNT_1_BIT,
		.tiling = VK_IMAGE_TILING_OPTIMAL,
		.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
		.queueFamilyIndexCount = 1,
		.pQueueFamilyIndices = &PhysicalDevice::gQueueFamilyIndices[0],
		.initialLayout = VK_IMAGE_LAYOUT_GENERAL,
	};

	VK_CHECK(vkCreateImage(gDevice, &create, nullptr, &image), "createImage: failed")

	return image;
}

VkImageView DepthImage::createImageView(VkImage image) {
	VkImageView view{};

	VkImageViewCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_D32_SFLOAT,
		.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1)
	};

	VK_CHECK(vkCreateImageView(gDevice, &create, nullptr, &view), "createImageView: failed")

	return view;
}
