#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "VulkanBackend.hpp"

namespace GlobalCreateInfos {
    inline uint16_t gWindowWidth{};
    inline uint16_t gWindowHeight{};
    inline const char* gWindowName{};
    inline VkApplicationInfo gAppInfo{};
    inline std::vector<const char*> gEnabledLoaderLayers{};
    inline std::vector<const char*> gEnabledInstanceExtensions{};
    inline VkInstanceCreateInfo gInstanceCreateInfo{};
    inline VkDeviceCreateInfo gLogicalDeviceCreateInfo{};

    [[nodiscard]] VkPhysicalDevice fSelectPhysicalDevice(VkInstance instance);
    [[nodiscard]] uint32_t fGetPhysicalDeviceGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice);
}