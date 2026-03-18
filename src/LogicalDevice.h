#include <array>
#include <vector>
#include "VChain.hpp"
#include "Util.h"

namespace Backend {
	namespace LogicalDevice {
		inline VkLogicalDevice gpDevice{};
		inline std::vector<VkQueue> gQueues{};

		inline std::vector<const char*> gExtensions{
			"VK_KHR_swapchain",
			"VK_KHR_synchronization2",
			"VK_KHR_spirv_1_4",
			#ifdef __APPLE__
			"VK_KHR_portability_subset"
			#endif
		};
		inline VChain<VkPhysicalDeviceFeatures2, 
		VkPhysicalDeviceBufferDeviceAddressFeatures, 
		VkPhysicalDeviceTimelineSemaphoreFeatures, 
		VkPhysicalDeviceSynchronization2Features, 
		VkPhysicalDeviceDynamicRenderingFeatures, 
		VkPhysicalDeviceExtendedDynamicState2FeaturesEXT> gFeatures(
			VkPhysicalDeviceFeatures2{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
				.features = {
					.samplerAnisotropy = true,
					.textureCompressionBC = true
				}
			},
			VkPhysicalDeviceBufferDeviceAddressFeatures{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES,
				.bufferDeviceAddress = true
			},
			VkPhysicalDeviceTimelineSemaphoreFeatures{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
				.timelineSemaphore = true
			},
			VkPhysicalDeviceSynchronization2Features{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
				.synchronization2 = true
			},
			VkPhysicalDeviceDynamicRenderingFeatures{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
				.dynamicRendering = true
			},
			VkPhysicalDeviceExtendedDynamicState2FeaturesEXT{
				.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_EXTENDED_DYNAMIC_STATE_2_FEATURES_EXT,
				.extendedDynamicState2 = true
			}
		);
		inline constexpr uint32_t gQUEUE_FAMILY_COUNT = 1;
		inline std::array<uint32_t, gQUEUE_FAMILY_COUNT> gQueueFamilyIndices{
			UINT32_MAX /* (ac) */
		};
		inline constexpr std::array<uint32_t, gQUEUE_FAMILY_COUNT> gQUEUES_PER_QUEUE_FAMILY{
			1
		};
		inline const std::array<std::vector<float>, gQUEUE_FAMILY_COUNT> gQUEUE_PRIORITIES{
			std::vector<float>{0.5f}
		};

		void init();
		void deInit();

		void createLogicalDevice();
		void createQueues();
		void destroyLogicalDevice() noexcept;
	}
}