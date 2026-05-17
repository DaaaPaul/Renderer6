#include "DepthImage.hpp"
#include "Utility.h"
#include "PhysicalDevice.h"

DepthImage::DepthImage(VkExtent3D extent) :
	image{ create_image(extent) }, image_view{ create_image_view(image) } {
	vkGetImageMemoryRequirements(g_device, image, &memory_requirements);
}

DepthImage::~DepthImage() {
	vkDestroyImageView(g_device, image_view, nullptr);
	vkDestroyImage(g_device, image, nullptr);
}

VkImage DepthImage::create_image(VkExtent3D extent) {
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
		.pQueueFamilyIndices = &PhysicalDevice::get_queue_family_index(VK_QUEUE_GRAPHICS_BIT),
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
	};

	VK_CHECK(vkCreateImage(g_device, &create, nullptr, &image), "create_image: failed")

	return image;
}

VkImageView DepthImage::create_image_view(VkImage image) {
	VkImageView view{};

	VkImageViewCreateInfo create{
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = VK_FORMAT_D32_SFLOAT,
		.subresourceRange = VkImageSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT, 0, 1, 0, 1)
	};

	VK_CHECK(vkCreateImageView(g_device, &create, nullptr, &view), "create_image_view: failed")

	return view;
}
