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

    private:
    VkInstance mInstance{};
    GlfwWindowWrapper* mGlfwWindowWrapper{};

    VkInstanceCreateInfo const* mINSTANCE_CREATE_INFO{};

    void arise();
    
    void setVulkanPFNsInstanceInUseToInstanceMember() const;

    static void checkHaveInstanceExtensions(std::vector<std::string> const& checkHaveMe);
    static void checkHaveLoaderLayers(std::vector<std::string> const& checkHaveMe);

    public:
    VulkanBackendWrapper(GlfwWindowWrapper* givenGlfwWindowWrapper, VkInstanceCreateInfo const* GIVEN_INSTANCE_CREATE_INFO);
    ~VulkanBackendWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanBackendWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanBackendWrapper)
};