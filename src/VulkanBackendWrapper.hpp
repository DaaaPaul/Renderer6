#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Common.h"
#include "GlfwWindowWrapper.hpp"

class VulkanBackendWrapper {
    friend class VulkanDevicesWrapper;
    friend class VulkanSwapchainWrapper;

    struct VulkanBackendWrapperConstructParameters {
        const std::vector<const char*> mENABLED_LOADER_LAYERS{};
        const std::vector<const char*> mENABLED_EXTENSIONS{};
        const VkApplicationInfo mAPP_INFO{};
        VkInstanceCreateInfo mCreateInfo{};
    };

    private:
    VkInstance const mInstance{};
    GlfwWindowWrapper* mGlfwWindowWrapper{};
    VulkanBackendWrapperConstructParameters mParameters{};

    void arise();
    
    void setVulkanPFNsInstanceInUseToInstanceMember() const;

    static void checkHaveInstanceExtensions(std::vector<const char*> const& CHECK_HAVE_ME);
    static void checkHaveLoaderLayers(std::vector<const char*> const& CHECK_HAVE_ME);

    public:
    VulkanBackendWrapper(GlfwWindowWrapper* givenGlfwWindowWrapper, VulkanBackendWrapperConstructParameters const& GIVEN_VULKAN_BACKEND_CREATE_PARAMETERS);
    ~VulkanBackendWrapper();

    static VulkanBackendWrapperConstructParameters getConstructParameters();
    [[nodiscard]] inline VkInstance getVkInstance() noexcept { return mInstance; }

    DELETE_COPY_CONSTRUCTORS(VulkanBackendWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanBackendWrapper)
};