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
#include "VulkanHostVisibleMemory.hpp"

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
		const std::vector<uint32_t> INDICES{
			0, 1, 3,
			0, 3, 2
		};
		const std::vector<VulkanBufferInfo> VERTEX_AND_INDEX_BUFFER_INFO{
			VulkanBufferInfo(sizeof(glm::vec4) * VERTEX_POSITIONS.size(), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mPhysicalDevice)),
			VulkanBufferInfo(sizeof(uint32_t) * INDICES.size(), VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mPhysicalDevice)),
		};
		VulkanHostVisibleMemory vulkanHostVisibleMemory(&vulkanDevicesWrapper, VERTEX_AND_INDEX_BUFFER_INFO);

		vulkanHostVisibleMemory.writeToBuffer(0, VERTEX_POSITIONS.data(), sizeof(glm::vec4) * VERTEX_POSITIONS.size());
		vulkanHostVisibleMemory.writeToBuffer(1, INDICES.data(), sizeof(uint32_t) * INDICES.size());
	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cout << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}