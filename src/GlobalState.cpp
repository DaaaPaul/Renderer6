#include "Common.h"
#include "GlobalState.h"

namespace GlobalState {
	const DeviceMemory::Common::HostVisibleConstructArguements fGetHostVisibleMemoryConstructArguements() {
		DeviceMemory::Common::HostVisibleConstructArguements constructArguements{};

		{
			constructArguements.mBufferInfos.emplace_back(32 * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
			constructArguements.mBufferInfos.emplace_back(4 * 6, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
			constructArguements.mBufferInfos.emplace_back(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
			constructArguements.mBufferInfos.emplace_back(gpKTX_TEXTURE->dataSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
		}

		{
			constructArguements.mDescriptorSetInfos.emplace_back(Vertex::Transforms::sGetTransformationMatricesDescriptorSetLayoutBinding(0));
		}

		return constructArguements;
	}

	void fPopulateHostVisibleMemory(DeviceMemory::HostVisible& toBePopulated) {
		const std::vector<Vertex::Vertex> VERTICIES{
			Vertex::Vertex(glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)), // top left
			Vertex::Vertex(glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)), // top right
			Vertex::Vertex(glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)), // bottom left
			Vertex::Vertex(glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)), // bottom right
		};
		const std::vector<uint32_t> INDICES{
			0, 1, 3,
			0, 3, 2
		};

		toBePopulated.writeToBuffer(0, VERTICIES.data(), 32 * 4);
		toBePopulated.writeToBuffer(1, INDICES.data(), 4 * 6);
		toBePopulated.writeToBuffer(3, gpKTX_TEXTURE->pData, gpKTX_TEXTURE->dataSize);
		toBePopulated.updateDescriptorSet(0, 0, {2});
	}

	const DeviceMemory::Common::DeviceLocalConstructArguements fGetDeviceLocalMemoryConstructArguements() {
		DeviceMemory::Common::DeviceLocalConstructArguements constructArguements{};

		{
			constructArguements.mBufferInfos.emplace_back(32 * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
			constructArguements.mBufferInfos.emplace_back(4 * 6, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
			constructArguements.mImageInfos.emplace_back(
				VK_IMAGE_TYPE_2D,
				gpKTX_TEXTURE->vkFormat,
				VkExtent3D(gpKTX_TEXTURE->baseWidth, gpKTX_TEXTURE->baseHeight, 1),
				1,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
				gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX,
				VK_IMAGE_LAYOUT_UNDEFINED
			);
		}

		return constructArguements;
	}

	void fPopulateDeviceLocalMemory(DeviceMemory::DeviceLocal& toBePopulated) {
		toBePopulated.copyBufferToBuffer(0, gHostVisibleMemory.mHostVisiblepBuffers[0], {VkBufferCopy(0, 0, 32 * 4)});
		toBePopulated.copyBufferToBuffer(1, gHostVisibleMemory.mHostVisiblepBuffers[1], {VkBufferCopy(0, 0, 4 * 6)});
		toBePopulated.copyBufferToImage(0, gHostVisibleMemory.mHostVisiblepBuffers[3], {VkBufferImageCopy(0, 0, 0, VkImageSubresourceLayers(VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1), VkOffset3D(0, 0, 0), VkExtent3D(gpKTX_TEXTURE->baseWidth, gpKTX_TEXTURE->baseHeight, 1))}); // buffer is tightly packed row by row
	}
}