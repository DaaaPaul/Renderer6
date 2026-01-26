#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "VulkanBackend.hpp"

namespace GlobalCreateInfos {
    inline VkApplicationInfo appInfo{};
    inline std::vector<const char*> enabledLoaderLayers{};
    inline std::vector<const char*> enabledInstanceExtensions{};
    inline VkInstanceCreateInfo instanceCreateInfo{};
}