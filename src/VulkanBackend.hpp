#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Common.h"
#include "Window.hpp"

class VulkanBackend {
private:
    VkInstance instance;
    Window* window;
    VkSurfaceKHR surface;

    VkInstanceCreateInfo const* INSTANCE_CREATE_INFO;

    void arise();
    void checkHaveInstanceExtensions(std::vector<std::string> const& checkHaveMe) const;
    void checkHaveLoaderLayers(std::vector<std::string> const& checkHaveMe) const;
    
public:
    VulkanBackend(Window* window, VkInstanceCreateInfo const* GIVEN_INSTANCE_CREATE_INFO);
    ~VulkanBackend();

    static std::vector<const char*> getGlfwWindowExtensions();

    DELETE_COPY_CONSTRUCTORS(VulkanBackend)
    DELETE_MOVE_CONSTRUCTORS(VulkanBackend)
};