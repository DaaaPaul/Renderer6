#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <iostream>
#include <stdexcept>
#include "VulkanPFNs.h"
#include "GlfwWindowWrapper.hpp"
#include "VulkanBackendWrapper.hpp"
#include "VulkanDevicesWrapper.hpp"
#include "VulkanSwapchainWrapper.hpp"
#include "VulkanMemoryCommon.h"
#include "VulkanHostVisibleMemory.hpp"
#include "VulkanDeviceLocalMemory.hpp"

int main() {
    try {
        VulkanPFNs::fLoadVkGetInstanceProcAddr();
        VulkanPFNs::fLoadTrueGlobalVulkanFunctions();

        GlfwWindowWrapper glfwWindowWrapper(GlfwWindowWrapper::getConstructParameters());

        VulkanBackendWrapper vulkanBackendWrapper(&glfwWindowWrapper, VulkanBackendWrapper::getConstructParameters());
        VulkanPFNs::gInstanceInUse = vulkanBackendWrapper.mInstance;
        VulkanPFNs::fLoadVulkanFunctions();

        VulkanDevicesWrapper vulkanDevicesWrapper(&vulkanBackendWrapper, VulkanDevicesWrapper::getConstructParameters(vulkanBackendWrapper.mInstance));
        
        VulkanSwapchainWrapper vulkanSwapchainWrapper(&vulkanDevicesWrapper, VulkanSwapchainWrapper::getConstructParameters(vulkanBackendWrapper.mInstance, vulkanDevicesWrapper.mPhysicalDevice, vulkanBackendWrapper.mGlfwWindowWrapper->mGlfwWindow));
    
		const std::vector<glm::vec4> VERTEX_POSITIONS{
			glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), // top left
			glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f), // top right
			glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), // bottom left
			glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), // bottom right
		};
		const VkDeviceSize VERTEX_POSITIONS_SIZE{ sizeof(glm::vec4) * VERTEX_POSITIONS.size() };
		const std::vector<uint32_t> INDICES{
			0, 1, 3,
			0, 3, 2
		};
		const VkDeviceSize INDICES_SIZE{ sizeof(uint32_t) * INDICES.size() };
		const std::vector<VulkanMemoryCommon::VulkanBufferInfo> STAGING_BUFFERS_INFO{
			VulkanMemoryCommon::VulkanBufferInfo(VERTEX_POSITIONS_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mPhysicalDevice)),
			VulkanMemoryCommon::VulkanBufferInfo(INDICES_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mPhysicalDevice)),
		};
		VulkanHostVisibleMemory vulkanHostVisibleMemory(&vulkanDevicesWrapper, STAGING_BUFFERS_INFO);
		vulkanHostVisibleMemory.writeToBuffer(0, VERTEX_POSITIONS.data(), sizeof(glm::vec4) * VERTEX_POSITIONS.size());
		vulkanHostVisibleMemory.writeToBuffer(1, INDICES.data(), sizeof(uint32_t) * INDICES.size());

		const std::vector<VulkanMemoryCommon::VulkanBufferInfo> VERTEX_AND_INDICE_BUFFERS_INFO{
			VulkanMemoryCommon::VulkanBufferInfo(VERTEX_POSITIONS_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mPhysicalDevice)),
			VulkanMemoryCommon::VulkanBufferInfo(INDICES_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mPhysicalDevice)),
		};
		VulkanDeviceLocalMemory vulkanDeviceLocalMemory(&vulkanDevicesWrapper, VERTEX_AND_INDICE_BUFFERS_INFO);
		vulkanDeviceLocalMemory.copyToBuffer(0, vulkanHostVisibleMemory.mHostVisibleBuffers[0], {VkBufferCopy(0, 0, VERTEX_POSITIONS_SIZE)});
		vulkanDeviceLocalMemory.copyToBuffer(1, vulkanHostVisibleMemory.mHostVisibleBuffers[1], {VkBufferCopy(0, 0, INDICES_SIZE)});
	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cout << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}