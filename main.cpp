#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <windows.h>
#include <iostream>

int main() {
    HMODULE pVulkanLoader = LoadLibraryA("vulkan-1.dll");
    if(!pVulkanLoader) {
        std::cout << "Failed to load vulkan loader!\n";
    }

    PFN_vkGetInstanceProcAddr pVkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(GetProcAddress(pVulkanLoader, "vkGetInstanceProcAddr"));
    PFN_vkCreateInstance pVkCreateInstance = reinterpret_cast<PFN_vkCreateInstance>(pVkGetInstanceProcAddr(nullptr, "vkCreateInstance"));

    if(!pVkGetInstanceProcAddr || !pVkCreateInstance) {
        std::cout << "I failed!\n";
    }

    return 0;
}