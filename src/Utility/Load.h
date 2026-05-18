#include <vulkan/vulkan.h>
#include "Backend/Instance.h"

namespace Load {
	inline PFN_vkTransitionImageLayoutEXT vkTransitionImageLayoutEXT = reinterpret_cast<PFN_vkTransitionImageLayoutEXT>(vkGetInstanceProcAddr(Instance::g_instance, "vkTransitionImageLayoutEXT"));
}
