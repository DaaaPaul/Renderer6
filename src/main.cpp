#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "VulkanPFNs.h"

int main() {
    loadVkGetInstanceProcAddr();
    loadVulkanFunctions();

    return 0;
}