#pragma once

#include <vulkan/vulkan.h>
#include <string>

namespace VulkanPFNs {
    void fLoadVkGetInstanceProcAddr();
    void fLoadTrueGlobalVulkanFunctions();
    void fLoadVulkanFunctions();

    inline VkInstance gpInstanceUsed{};

    inline PFN_vkGetInstanceProcAddr gpVkGetInstanceProcAddr{};
    inline PFN_vkEnumerateInstanceExtensionProperties gpVkEnumerateInstanceExtensionProperties{};
    inline PFN_vkEnumerateInstanceLayerProperties gpVkEnumerateInstanceLayerProperties{};
    inline PFN_vkCreateInstance gpVkCreateInstance{};
    inline PFN_vkDestroyInstance gpVkDestroyInstance{};
    inline PFN_vkDestroySurfaceKHR gpVkDestroySurfaceKHR{};
    inline PFN_vkEnumeratePhysicalDevices gpVkEnumeratePhysicalDevices{};
    inline PFN_vkGetPhysicalDeviceProperties gpVkGetPhysicalDeviceProperties{};
    inline PFN_vkGetPhysicalDeviceQueueFamilyProperties gpVkGetPhysicalDeviceQueueFamilyProperties{};
    inline PFN_vkEnumerateDeviceExtensionProperties gpVkEnumerateDeviceExtensionProperties{};
    inline PFN_vkGetPhysicalDeviceFeatures2 gpVkGetPhysicalDeviceFeatures2{};
    inline PFN_vkCreateDevice gpVkCreateDevice{};
	inline PFN_vkGetDeviceQueue gpVkGetDeviceQueue{};
    inline PFN_vkDestroyDevice gpVkDestroyDevice{};
    inline PFN_vkGetPhysicalDeviceSurfaceCapabilitiesKHR gpVkGetPhysicalDeviceSurfaceCapabilitiesKHR{};
    inline PFN_vkGetPhysicalDeviceSurfaceFormatsKHR gpVkGetPhysicalDeviceSurfaceFormatsKHR{};
    inline PFN_vkGetPhysicalDeviceSurfacePresentModesKHR gpVkGetPhysicalDeviceSurfacePresentModesKHR{};
    inline PFN_vkCreateSwapchainKHR gpVkCreateSwapchainKHR{};
    inline PFN_vkDestroySwapchainKHR gpVkDestroySwapchainKHR{};
    inline PFN_vkGetSwapchainImagesKHR gpVkGetSwapchainImagesKHR{};
	inline PFN_vkCreateImageView gpVkCreateImageView{};
	inline PFN_vkDestroyImageView gpVkDestroyImageView{};
	inline PFN_vkCreateBuffer gpVkCreateBuffer{};
	inline PFN_vkDestroyBuffer gpVkDestroyBuffer{};
	inline PFN_vkGetPhysicalDeviceMemoryProperties gpVkGetPhysicalDeviceMemoryProperties{};
	inline PFN_vkGetBufferMemoryRequirements gpVkGetBufferMemoryRequirements{};
	inline PFN_vkAllocateMemory gpVkAllocateMemory{};
	inline PFN_vkBindBufferMemory gpVkBindBufferMemory{};
	inline PFN_vkMapMemory gpVkMapMemory{};
	inline PFN_vkUnmapMemory gpVkUnmapMemory{};
	inline PFN_vkFreeMemory gpVkFreeMemory{};
	inline PFN_vkCreateCommandPool gpVkCreateCommandPool{};
	inline PFN_vkDestroyCommandPool gpVkDestroyCommandPool{};
	inline PFN_vkAllocateCommandBuffers gpVkAllocateCommandBuffers{};
	inline PFN_vkFreeCommandBuffers gpVkFreeCommandBuffers{};
	inline PFN_vkBeginCommandBuffer gpVkBeginCommandBuffer{};
	inline PFN_vkCmdCopyBuffer gpVkCmdCopyBuffer{};
	inline PFN_vkEndCommandBuffer gpVkEndCommandBuffer{};
	inline PFN_vkCreateFence gpVkCreateFence{};
	inline PFN_vkDestroyFence gpVkDestroyFence{};
	inline PFN_vkQueueSubmit gpVkQueueSubmit{};
	inline PFN_vkWaitForFences gpVkWaitForFences{};
	inline PFN_vkCreateSemaphore gpVkCreateSemaphore{};
	inline PFN_vkDestroySemaphore gpVkDestroySemaphore{};
	inline PFN_vkCreateShaderModule gpVkCreateShaderModule{};
	inline PFN_vkCreatePipelineLayout gpVkCreatePipelineLayout{};
	inline PFN_vkCreateGraphicsPipelines gpVkCreateGraphicsPipelines{};
	inline PFN_vkDestroyPipeline gpVkDestroyPipeline{};
	inline PFN_vkDestroyPipelineLayout gpVkDestroyPipelineLayout{};
	inline PFN_vkDestroyShaderModule gpVkDestroyShaderModule{};
	inline PFN_vkAcquireNextImageKHR gpVkAcquireNextImageKHR{};
	inline PFN_vkCmdPipelineBarrier2 gpVkCmdPipelineBarrier2{};
	inline PFN_vkCmdBindPipeline gpVkCmdBindPipeline{};
	inline PFN_vkCmdBindIndexBuffer gpVkCmdBindIndexBuffer{};
	inline PFN_vkCmdBindVertexBuffers gpVkCmdBindVertexBuffers{};
	inline PFN_vkCmdSetViewport gpVkCmdSetViewport{};
	inline PFN_vkCmdSetScissor gpVkCmdSetScissor{};
	inline PFN_vkCmdBeginRendering gpVkCmdBeginRendering{};
	inline PFN_vkCmdEndRendering gpVkCmdEndRendering{};
	inline PFN_vkCmdDrawIndexed gpVkCmdDrawIndexed{};
	inline PFN_vkQueuePresentKHR gpVkQueuePresentKHR{};
	inline PFN_vkResetFences gpVkResetFences{};
	inline PFN_vkDeviceWaitIdle gpVkDeviceWaitIdle{};
	inline PFN_vkResetCommandBuffer gpVkResetCommandBuffer{};
	inline PFN_vkCreateDescriptorPool gpVkCreateDescriptorPool{};
	inline PFN_vkCreateDescriptorSetLayout gpVkCreateDescriptorSetLayout{};
	inline PFN_vkAllocateDescriptorSets gpVkAllocateDescriptorSets{};
	inline PFN_vkFreeDescriptorSets gpVkFreeDescriptorSets{};
	inline PFN_vkDestroyDescriptorSetLayout gpVkDestroyDescriptorSetLayout{};
	inline PFN_vkDestroyDescriptorPool gpVkDestroyDescriptorPool{};
	inline PFN_vkUpdateDescriptorSets gpVkUpdateDescriptorSets{};
}