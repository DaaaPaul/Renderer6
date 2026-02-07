#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include <iostream>
#include <stdexcept>
#include "VulkanPFNs.h"
#include "Vertex.hpp"
#include "TransformationMatrices.hpp"
#include "GlfwWindowWrapper.hpp"
#include "VulkanBackendWrapper.hpp"
#include "VulkanDevicesWrapper.hpp"
#include "VulkanSwapchainWrapper.hpp"
#include "VulkanMemoryCommon.h"
#include "VulkanHostVisibleMemory.hpp"
#include "VulkanDeviceLocalMemory.hpp"
#include "VulkanGraphicsPipelineWrapper.hpp"
#include "RenderEngine.h"

int main() {
    try {
        VulkanPFNs::fLoadVkGetInstanceProcAddr();
        VulkanPFNs::fLoadTrueGlobalVulkanFunctions();

        GlfwWindowWrapper glfwWindowWrapper(GlfwWindowWrapper::getConstructParameters());

        VulkanBackendWrapper vulkanBackendWrapper(&glfwWindowWrapper, VulkanBackendWrapper::getConstructParameters());
        VulkanPFNs::gpInstanceUsed = vulkanBackendWrapper.mpInstance;
        VulkanPFNs::fLoadVulkanFunctions();

        VulkanDevicesWrapper vulkanDevicesWrapper(&vulkanBackendWrapper, VulkanDevicesWrapper::getConstructParameters(vulkanBackendWrapper.mpInstance));

        VulkanSwapchainWrapper vulkanSwapchainWrapper(&vulkanDevicesWrapper, VulkanSwapchainWrapper::getConstructParameters(vulkanBackendWrapper.mpInstance, vulkanDevicesWrapper.mpPhysicalDevice, vulkanBackendWrapper.mpGlfwWindowWrapper->mpGlfwWindow));

		const std::vector<Vertex> VERTICIES{
			Vertex(glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)), // top left
			Vertex(glm::vec4(0.5f, -0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)), // top right
			Vertex(glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f)), // bottom left
			Vertex(glm::vec4(0.5f, 0.5f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f)), // bottom right
		};
		const VkDeviceSize VERTICIES_SIZE{ sizeof(Vertex) * VERTICIES.size() };
		const std::vector<uint32_t> INDICES{
			0, 1, 3,
			0, 3, 2
		};
		const VkDeviceSize INDICES_SIZE{ sizeof(uint32_t) * INDICES.size() };

		const std::vector<VulkanMemoryCommon::BufferInfo> HOST_VISIBLE_BUFFERS_INFO{
			VulkanMemoryCommon::BufferInfo(VERTICIES_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mpPhysicalDevice)),
			VulkanMemoryCommon::BufferInfo(INDICES_SIZE, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mpPhysicalDevice)),
			VulkanMemoryCommon::BufferInfo(sizeof(TransformationMatrices), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mpPhysicalDevice))
		};
		const VkDescriptorSetLayoutBinding uniformBufferLayoutBinding{ TransformationMatrices::getTransformationMatricesDescriptorSetLayoutBinding(0) };
		const std::vector<VulkanMemoryCommon::DescriptorSetInfo> DESCRIPTOR_SET_INFO{
			VulkanMemoryCommon::DescriptorSetInfo(1, &uniformBufferLayoutBinding)
		};
		VulkanHostVisibleMemory vulkanHostVisibleMemory(&vulkanDevicesWrapper, HOST_VISIBLE_BUFFERS_INFO, DESCRIPTOR_SET_INFO);
		vulkanHostVisibleMemory.writeToBuffer(0, VERTICIES.data(), VERTICIES_SIZE);
		vulkanHostVisibleMemory.writeToBuffer(1, INDICES.data(), INDICES_SIZE);
		vulkanHostVisibleMemory.updateDescriptorSet(0, 0, {2});

		const std::vector<VulkanMemoryCommon::BufferInfo> DEVICE_LOCAL_BUFFERS_INFO{
			VulkanMemoryCommon::BufferInfo(VERTICIES_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mpPhysicalDevice)),
			VulkanMemoryCommon::BufferInfo(INDICES_SIZE, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mpPhysicalDevice)),
		};

		VulkanDeviceLocalMemory vulkanDeviceLocalMemory(&vulkanDevicesWrapper, DEVICE_LOCAL_BUFFERS_INFO, {});
		vulkanDeviceLocalMemory.copyToBuffer(0, vulkanHostVisibleMemory.mHostVisiblepBuffers[0], {VkBufferCopy(0, 0, VERTICIES_SIZE)});
		vulkanDeviceLocalMemory.copyToBuffer(1, vulkanHostVisibleMemory.mHostVisiblepBuffers[1], {VkBufferCopy(0, 0, INDICES_SIZE)});

		const VkPipelineLayoutCreateInfo PIPELINE_LAYOUT_INFO{
			.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
			.setLayoutCount = 1,
			.pSetLayouts = &vulkanHostVisibleMemory.mDescriptorpSetLayouts[0],
		};
		VulkanGraphicsPipelineWrapper vulkanGraphicsPipelineWrapper(&vulkanDevicesWrapper, VulkanGraphicsPipelineWrapper::getConstructParameters(PIPELINE_LAYOUT_INFO));

		RenderEngine::gpVulkanGraphicsPipelineWrapper = &vulkanGraphicsPipelineWrapper;
		RenderEngine::gpVulkanSwapchainWrapper = &vulkanSwapchainWrapper;
		RenderEngine::gpVulkanDeviceLocalMemory = &vulkanDeviceLocalMemory;
		RenderEngine::gpVulkanHostVisibleMemory = &vulkanHostVisibleMemory;
		RenderEngine::ImageKillhouse killhouse(vulkanSwapchainWrapper.mParameters.mSwapchainKHRCreateInfo.minImageCount, VulkanDevicesWrapper::getGraphicsQueueFamilyIndex(vulkanDevicesWrapper.mpPhysicalDevice));
		RenderEngine::fRenderLoop(killhouse);
	} catch(std::runtime_error const& RUNTIME_ERROR) {
        std::cout << "ERROR: " << RUNTIME_ERROR.what() << "\n";
    }

    return 0;
}