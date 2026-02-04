#include <iostream>
#include "RenderEngine.h"

namespace RenderEngine {
	ImageKiller::ImageHitmanEquipment::ImageHitmanEquipment(VkCommandPool pool) :
	mRenderReady{},
	mRenderFinished{},
	mCommandPool{ pool },
	mDrawCommands{} {

		std::cout << "Creating ImageHitmanEquipment...\n";

		// create semaphores
		{
			const VkSemaphoreCreateInfo EMPTY_INFO{
				.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
			};
			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkCreateSemaphore(gpLogicalDeviceUsed, &EMPTY_INFO, nullptr, &mRenderReady),
				"Semaphore creation failed"
			)
			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkCreateSemaphore(gpLogicalDeviceUsed, &EMPTY_INFO, nullptr, &mRenderFinished),
				"Semaphore creation failed"
			)
		}

		// create command buffer
		{
			const VkCommandBufferAllocateInfo DRAW_COMMANDS_INFO{
				.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
				.commandPool = mCommandPool,
				.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
				.commandBufferCount = 1,
			};

			CHECK_VK_SUCCESS(
				VulkanPFNs::gpVkAllocateCommandBuffers(gpLogicalDeviceUsed, &DRAW_COMMANDS_INFO, &mDrawCommands),
				"Command buffer creation failed"
			)
		}

		std::cout << "Created ImageHitmanEquipment\n";
	}

	ImageKiller::ImageHitmanEquipment::~ImageHitmanEquipment() {
		std::cout << "Destroying ImageHitmanEquipment...\n";

		VulkanPFNs::gpVkDestroySemaphore(gpLogicalDeviceUsed, mRenderReady, nullptr);
		VulkanPFNs::gpVkDestroySemaphore(gpLogicalDeviceUsed, mRenderFinished, nullptr);
		VulkanPFNs::gpVkFreeCommandBuffers(gpLogicalDeviceUsed, mCommandPool, 1, &mDrawCommands);

		std::cout << "Destroyed ImageHitmanEquipment\n";
	}

	ImageKiller::ImageKiller(uint16_t const& HITMEN_COUNT, uint32_t const& GRAPHICS_QF_INDEX) : 
	mpCommandPoolUsed{},
	mHitmen{} {
		const VkCommandPoolCreateInfo POOL_INFO{
			.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
			.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT,
			.queueFamilyIndex = GRAPHICS_QF_INDEX
		};

		CHECK_VK_SUCCESS(
			VulkanPFNs::gpVkCreateCommandPool(gpLogicalDeviceUsed, &POOL_INFO, nullptr, &mpCommandPoolUsed),
			"Failed to create command pool"
		)

		mHitmen.reserve(HITMEN_COUNT);
		for(int i = 0; i < HITMEN_COUNT; i++) {
			mHitmen.emplace_back(mpCommandPoolUsed);
		}
	}

	ImageKiller::~ImageKiller() {
		mHitmen.clear();
		VulkanPFNs::gpVkDestroyCommandPool(gpLogicalDeviceUsed, mpCommandPoolUsed, nullptr);
	}

	void fRenderLoop() {
		while(!glfwWindowShouldClose(gpGlfwWindowUsed)) {
			glfwPollEvents();

		}
	}
}
