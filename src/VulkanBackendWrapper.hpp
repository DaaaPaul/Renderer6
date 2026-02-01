#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Common.h"
#include "GlfwWindowWrapper.hpp"

struct VulkanBackendWrapper {
    struct VulkanBackendWrapperConstructParameters {
        const std::vector<const char*> mENABLED_LOADER_LAYERS{};
        const std::vector<const char*> mENABLED_EXTENSIONS{};
        const VkApplicationInfo mAPP_INFO{};
        VkInstanceCreateInfo mCreateInfo{};
    };

    VkInstance mpInstance{};
    GlfwWindowWrapper* mpGlfwWindowWrapper{};
    VulkanBackendWrapperConstructParameters mParameters{};

    static void checkHaveInstanceExtensions(std::vector<const char*> const& CHECK_HAVE_ME);
    static void checkHaveLoaderLayers(std::vector<const char*> const& CHECK_HAVE_ME);
    [[nodiscard]] static VulkanBackendWrapperConstructParameters getConstructParameters();

    explicit VulkanBackendWrapper(GlfwWindowWrapper* givenGlfwWindowWrapper, VulkanBackendWrapperConstructParameters const& GIVEN_VULKAN_BACKEND_CREATE_PARAMETERS);
    ~VulkanBackendWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanBackendWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanBackendWrapper)
};