#include "Memory.hpp"

namespace MemoryManager {
	inline std::vector<Texture> gTextures{};
	inline std::vector<DepthImage> gDepthImages{};
	inline std::vector<Buffer> gBuffers{};
	inline std::vector<DescriptorSet> gDescriptorSets{};

	inline Memory gHost{};
	inline Memory g_device{};

	void init();

	void addTextures();
	void addDepthImages();
	void addBuffers();
	void addDescriptorSets();
	void createHost();
	void createDevice();
}