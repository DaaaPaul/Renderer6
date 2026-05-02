#include "Memory.hpp"
#include "PhysicalDevice.h"

namespace Resource {
	Memory::Memory(std::vector<Texture> const& TEXTURES, std::vector<Buffer> const& BUFFERS, std::vector<DescriptorSet> const& SETS, VkMemoryPropertyFlags propertyFlags) {
	
	}

	Memory::~Memory() {
	
	}

	std::pair<VkDeviceSize, std::vector<VkDeviceSize>> Memory::sizeAndOffsets(std::vector<Texture> const& TEXTURES, std::vector<Buffer> const& BUFFERS) {
		std::vector<VkDeviceSize> offsets{};
		VkDeviceSize running = 0;

		for(Texture const& TEXT : TEXTURES) {
			running = alignNext(running, TEXT.getRequirements().alignment);
			offsets.push_back(running);
			running += TEXT.getRequirements().size;
		}

		alignNext(running, Backend::PhysicalDevice::gLimits.bufferImageGranularity);

		for(Buffer const& BUF : BUFFERS) {
			running = alignNext(running, BUF.getRequirements().alignment);
			offsets.push_back(running);
			running += BUF.getRequirements().size;
		}

		return { running, offsets };
	}

	void Memory::copyToImage(ktxTexture2* texture, VkImage image) {
		VkHostImageLayoutTransitionInfo transition{
			.sType = VK_STRUCTURE_TYPE_HOST_IMAGE_LAYOUT_TRANSITION_INFO,
			.image = image,
			.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
			.newLayout = VK_IMAGE_LAYOUT_GENERAL,
			.subresourceRange = VkImageSubresourceRange{VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1}
		};
		VK_CHECK(vkTransitionImageLayoutEXT(gDevice, 1, &transition), "copyToImage: transition error")

		VkMemoryToImageCopyEXT region{
			.sType = VK_STRUCTURE_TYPE_MEMORY_TO_IMAGE_COPY_EXT,
			.pHostPointer = texture->pData,
			.imageSubresource = VkImageSubresourceLayers{VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1},
			.imageExtent = VkExtent3D{texture->baseWidth, texture->baseHeight, texture->baseDepth}
		};

		VkCopyMemoryToImageInfoEXT copy{
			.sType = VK_STRUCTURE_TYPE_COPY_MEMORY_TO_IMAGE_INFO_EXT,
			.dstImage = image,
			.dstImageLayout = VK_IMAGE_LAYOUT_GENERAL,
			.regionCount = 1,
			.pRegions = &region,
		};

		VK_CHECK(vkCopyMemoryToImage(gDevice, &copy), "copyToImage: copy error");
	}
}