#include <vulkan/vulkan.h>
#include "Common.h"
#include "VulkanDevicesWrapper.hpp"

class VulkanSwapchainWrapper {
    private:
    VulkanDevicesWrapper* mVulkanDevicesWrapper{};
    VkSwapchainKHR mSwapchainKHR{};
    VkSurfaceKHR mSurfaceKHR{};

    VkSwapchainCreateInfoKHR const* mSWAPCHAIN_KHR_CREATE_INFO{};

    void arise();

    static void checkHaveVkFormat(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkSurfaceFormatKHR const& CHECK_ME_FORMAT_COLORSPACE);
    static void checkHavePresentModeKHR(VulkanSwapchainWrapper const& VULKAN_SWAPCHAIN_WRAPPER, VkPresentModeKHR const& CHECK_ME_PRESENT_MODE);

    public:
    VulkanSwapchainWrapper(VulkanDevicesWrapper* givenVulkanDevicesWrapper, VkSwapchainCreateInfoKHR const* GIVEN_SWAPCHAIN_CREATE_INFO);
    ~VulkanSwapchainWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanSwapchainWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanSwapchainWrapper)
};