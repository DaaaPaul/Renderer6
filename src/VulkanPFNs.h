#pragma once

#include <vulkan/vulkan.h>
#include <string>

void setInstance(VkInstance const& givenInstance);
void loadVkGetInstanceProcAddr();
void loadTrueGlobalVulkanFunctions();
void loadVulkanFunctions();

inline VkInstance instanceInUse{};

inline PFN_vkGetInstanceProcAddr pVkGetInstanceProcAddr{};
inline PFN_vkEnumerateInstanceExtensionProperties pVkEnumerateInstanceExtensionProperties{};
inline PFN_vkEnumerateInstanceLayerProperties pVkEnumerateInstanceLayerProperties{};
inline PFN_vkCreateInstance pVkCreateInstance{};
inline PFN_vkDestroyInstance pVkDestroyInstance{};