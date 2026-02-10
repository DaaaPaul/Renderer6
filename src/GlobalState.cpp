#include "Common.h"
#include "GlobalState.h"

namespace GlobalState {
	const DeviceMemory::Common::ConstructArguements fConstructHostVisibleMemory() {
		DeviceMemory::Common::ConstructArguements constructArguements{};
		constructArguements.mpDevices = &gDevicesWrapper;
		constructArguements.mBufferInfos.emplace_back(32 * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
		constructArguements.mBufferInfos.emplace_back(4 * 6, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
		constructArguements.mBufferInfos.emplace_back(sizeof(Vertex::Transforms), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
		
		const char* PATH_TO_TEXTURE{ R"(C:\Users\paulp\ComputerPrograms\Renderer6\resources\textures\Lumberjack Sion Compressed.ktx2)" };
		uint32_t texWidth{}, texHeight{};
		ktxTexture2* pKtxTexture{ DeviceMemory::Common::fKtxLoadImage(PATH_TO_TEXTURE, texWidth, texHeight, texSize, pTexData) };
		std::cout << "Loaded texture at " << PATH_TO_TEXTURE << " with: \n";
		std::cout << "Width " << texWidth << "\n";
		std::cout << "Height " << texHeight << "\n";
		std::cout << "Size " << texSize << "\n";
		std::cout << "Into address " << PTR_TO_DECIMAL(pTexData) << "\n";

		constructArguements.mBufferInfos.emplace_back(texSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, gDevicesWrapper.mGRAPHICS_QUEUE_FAMILY_INDEX);
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
		toBePopulated.writeToBuffer(3, pTexData, texSize);
		toBePopulated.updateDescriptorSet(0, 0, {2});
	}

	const DeviceMemory::Common::ConstructArguements fConstructDeviceLocalMemory() {
		gDeviceLocalMemory.copyToBuffer(0, gHostVisibleMemory.mHostVisiblepBuffers[0], {VkBufferCopy(0, 0, 32 * 4)});
		gDeviceLocalMemory.copyToBuffer(1, gHostVisibleMemory.mHostVisiblepBuffers[1], {VkBufferCopy(0, 0, 4 * 6)});
	}

	void fPopulateDeviceLocalMemory(DeviceMemory::DeviceLocal& toBePopulated) {
	
	}
}