#include "Common.h"
#include "GlobalState.h"

namespace GlobalState {
	inline void fLoadDeviceMemory() {
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

		gHostVisibleMemory.writeToBuffer(0, VERTICIES.data(), 32 * 4);
		gHostVisibleMemory.writeToBuffer(1, INDICES.data(), 4 * 6);
		gHostVisibleMemory.updateDescriptorSet(0, 0, {2});

		gDeviceLocalMemory.copyToBuffer(0, gHostVisibleMemory.mHostVisiblepBuffers[0], {VkBufferCopy(0, 0, 32 * 4)});
		gDeviceLocalMemory.copyToBuffer(1, gHostVisibleMemory.mHostVisiblepBuffers[1], {VkBufferCopy(0, 0, 4 * 6)});
	}
}