#include <vulkan/vulkan.h>
#include <vector>
#include <string>
#include "VulkanBackend.hpp"

namespace GlobalCreateInfos {
    inline uint16_t gWindowWidth{};
    inline uint16_t gWindowHeight{};
    inline const char* gWindowName{};

    inline VkInstanceCreateInfo gInstanceCreateInfo{};
    inline VkApplicationInfo gAppInfo{};
    inline std::vector<const char*> gEnabledLoaderLayers{};
    inline std::vector<const char*> gEnabledInstanceExtensions{};

    inline VkPhysicalDevice gSelectedPhysicalDevice{};

    inline VkDeviceCreateInfo gLogicalDeviceCreateInfo{};
    inline std::vector<VkDeviceQueueCreateInfo> gDeviceQueueFamilyCreateInfos{};
    inline std::vector<std::vector<float>> gDeviceQueueFamilyQueuePriorities{};
    inline std::vector<const char*> gEnabledDeviceExtensions{};
    inline VkPhysicalDeviceFeatures2 gEnabledDeviceFeatures{};

    void fPopulateGlobalWindowCreateInfo();
    void fPopulateGlobalInstanceCreateInfo();
    void fPopulateGlobalSharedPhysicalLogicalDeviceInfo();
    void fPopulateGlobalSelectedPhysicalDevice(VkInstance createdInstance);
    void fPopulateGlobalLogicalDeviceCreateInfo();

    [[nodiscard]] std::vector<const char*> fGetGlfwWindowExtensions();
    [[nodiscard]] VkPhysicalDevice fSelectPhysicalDevice(VkInstance instance);
    [[nodiscard]] uint32_t fGetGraphicsQueueFamilyIndex(VkPhysicalDevice physicalDevice);
}