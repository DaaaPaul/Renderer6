#include "Memory.hpp"

namespace MemoryManager {
	inline std::vector<Texture> gTextures{};
	inline std::vector<DepthImage> gDepthImages{};
	inline std::vector<Buffer> gBuffers{};
	inline std::vector<DescriptorSet> gDescriptorSets{};

	inline Memory g_host_memory{};
	inline Memory g_device_memory{};

	void init();

	void addTextures();
	void addDepthImages();
	void addBuffers();
	void addDescriptorSets();
	void createHost();
	void createDevice();
}