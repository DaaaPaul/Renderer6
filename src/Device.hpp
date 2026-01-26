#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanPFNs.h"
#include "VulkanBackend.hpp"

class Device {
private:
    VulkanBackend* vulkanBackend;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;

    VkDeviceCreateInfo const* LOGICAL_DEVICE_CREATE_INFO;

    void arise();

public:
    Device(VulkanBackend* givenVulkanBackend, VkPhysicalDevice const& GIVEN_PHYSICAL_DEVICE, VkDeviceCreateInfo const* GIVEN_LOGICAL_DEVICE_CREATE_INFO);
    ~Device();

    DELETE_COPY_CONSTRUCTORS(Device)
    DELETE_MOVE_CONSTRUCTORS(Device)
};