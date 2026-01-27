#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Common.h"
#include "GlfwWindowWrapper.hpp"

class VulkanBackendWrapper {
private:
    VkInstance instance;
    GlfwWindowWrapper* window;
    VkSurfaceKHR surface;

    VkInstanceCreateInfo const* INSTANCE_CREATE_INFO;

    void arise();
    void checkHaveInstanceExtensions(std::vector<std::string> const& checkHaveMe) const;
    void checkHaveLoaderLayers(std::vector<std::string> const& checkHaveMe) const;
    void setVulkanPFNsInstanceInUseToInstanceMember() const;

public:
    VulkanBackendWrapper(GlfwWindowWrapper* givenGlfwWindowWrapper, VkInstanceCreateInfo const* GIVEN_INSTANCE_CREATE_INFO);
    ~VulkanBackendWrapper();

    DELETE_COPY_CONSTRUCTORS(VulkanBackendWrapper)
    DELETE_MOVE_CONSTRUCTORS(VulkanBackendWrapper)
};