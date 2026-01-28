#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanDevicesWrapper.hpp"

class VulkanSwapchainWrapper {
    private:
    VulkanDevicesWrapper* mVulkanDevicesWrapper{};
    VkSwapchainKHR mSwapchainKHR{};

    VkSwapchainCreateInfoKHR const* mSWAPCHAIN_KHR_CREATE_INFO{};

    void arise();

    static void checkHaveVkFormat(VulkanDevicesWrapper const& VULKAN_DEVICES_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE);
    static void checkHavePresentModeKHR(VulkanDevicesWrapper const& VULKAN_DEVICES_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE);

    public:
    VulkanSwapchainWrapper(VkSwapchainCreateInfoKHR const* GIVEN_SWAPCHAIN_CREATE_INFO);
    ~VulkanSwapchainWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanSwapchainWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanSwapchainWrapper)
};