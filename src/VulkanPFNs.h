#pragma once

#include <vulkan/vulkan.h>
#include <string>

void loadVkGetInstanceProcAddr();
void loadVulkanFunctions();

inline PFN_vkGetInstanceProcAddr pVkGetInstanceProcAddr{};
inline PFN_vkCreateInstance pVkCreateInstance{};