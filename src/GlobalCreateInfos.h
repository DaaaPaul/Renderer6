#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "VulkanBackend.hpp"

namespace GlobalCreateInfos {
    inline VkApplicationInfo gAppInfo{};
    inline std::vector<const char*> gEnabledLoaderLayers{};
    inline std::vector<const char*> gEnabledInstanceExtensions{};
    inline VkInstanceCreateInfo gInstanceCreateInfo{};
}