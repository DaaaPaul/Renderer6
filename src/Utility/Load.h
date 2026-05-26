#include <vulkan/vulkan.h>
#include "Backend/Instance.h"

namespace Load {
	inline PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT{};
	inline PFN_vkCopyMemoryToImageEXT vkCopyMemoryToImageEXT{};

	inline void load() {
		vkTransitionImageLayoutEXT = reinterpret_cast<PFN_vkTransitionImageLayoutEXT>(vkGetInstanceProcAddr(Instance::g_instance, "vkTransitionImageLayoutEXT"));
		vkCopyMemoryToImageEXT = reinterpret_cast<PFN_vkCopyMemoryToImageEXT>(vkGetInstanceProcAddr(Instance::g_instance, "vkCopyMemoryToImageEXT"));
	}
}
