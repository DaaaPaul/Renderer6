#include "Memory.hpp"

namespace MemoryManager {
	inline std::vector<Texture> g_textures{};
	inline std::vector<DepthImage> g_depth_images{};
	inline std::vector<Buffer> g_buffers{};
	inline std::vector<DescriptorSet> g_descriptor_sets{};

	inline Memory g_device_host_memory{};
	inline Memory g_device_memory{};

	void init();
}