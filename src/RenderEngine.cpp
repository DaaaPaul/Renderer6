#include <iostream>
#include "RenderEngine.h"

namespace RenderEngine {
	ImageHitmanEquipment::ImageHitmanEquipment() :
	mRenderReady{},
	mRenderFinished{},
	mDrawCommands{} {

		std::cout << "Creating ImageHitmanEquipment...\n";

		// create semaphores
		{
			const VkSemaphoreCreateInfo EMPTY_INFO{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};
			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkCreateSemaphore(gLogicalDeviceUsed, &EMPTY_INFO, nullptr, &mRenderReady),
				"Semaphore creation failed"
			)
			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkCreateSemaphore(gLogicalDeviceUsed, &EMPTY_INFO, nullptr, &mRenderFinished),
				"Semaphore creation failed"
			)
		}

		// create command buffer
		{
			const VkCommandBufferAllocateInfo DRAW_COMMANDS_INFO{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = gCommandPoolUsed,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};

			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkAllocateCommandBuffers(gLogicalDeviceUsed, &DRAW_COMMANDS_INFO, &mDrawCommands),
				"Command buffer creation failed"
			)
		}

		std::cout << "Created ImageHitmanEquipment\n";
	}

	ImageHitmanEquipment::~ImageHitmanEquipment() {
		std::cout << "Destroying ImageHitmanEquipment...\n";

		VulkanPFNs::gpVkDestroySemaphore(gLogicalDeviceUsed, mRenderReady, nullptr);
		VulkanPFNs::gpVkDestroySemaphore(gLogicalDeviceUsed, mRenderFinished, nullptr);
		VulkanPFNs::gpVkFreeCommandBuffers(gLogicalDeviceUsed, gCommandPoolUsed, 1, &mDrawCommands);

		std::cout << "Destroyed ImageHitmanEquipment\n";
	}

	void fRenderLoop() {
		while(!glfwWindowShouldClose(gGlfwWindowUsed)) {
			glfwPollEvents();

		}
	}
}
