#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanBackendWrapper.hpp"

class VulkanDevicesWrapper {
private:
    VulkanBackendWrapper* vulkanBackend;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;

    VkDeviceCreateInfo const* LOGICAL_DEVICE_CREATE_INFO;

    void arise();

public:
    VulkanDevicesWrapper(VulkanBackendWrapper* givenVulkanBackendWrapper, VkPhysicalDevice const& GIVEN_PHYSICAL_DEVICE, VkDeviceCreateInfo const* GIVEN_LOGICAL_DEVICE_CREATE_INFO);
    ~VulkanDevicesWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanDevicesWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanDevicesWrapper)
};