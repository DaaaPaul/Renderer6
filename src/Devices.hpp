#pragma once

#include "Util.h"
#include "Instance.hpp"
#include "VChain.hpp"

namespace Backend {
	class Devices {
		public:
		struct CreateInfo {
			VkPhysicalDevice pPhysicalDevice{};
			VkDeviceCreateInfo logicalDeviceInfo{};
			std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos{};
			std::vector<std::vector<float>> queueFamilyPriorities{};
			std::vector<const char*> extensions{};
			VChain<VkPhysicalDeviceFeatures2, VkPhysicalDeviceBufferDeviceAddressFeatures, VkPhysicalDeviceTimelineSemaphoreFeatures, VkPhysicalDeviceSynchronization2Features, VkPhysicalDeviceDynamicRenderingFeatures, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT> features;

			void reroutePointers() {
				logicalDeviceInfo.pNext = &features.val;
				features.reroutePointers();
			}
			CreateInfo(VkPhysicalDevice&& givenPhysicalDevice, VkDeviceCreateInfo const& GIVEN_LOGICAL_DEVICE_INFO, std::vector<VkDeviceQueueCreateInfo>&& salvageQfInfos, std::vector<std::vector<float>>&& salvageQfPriorities, std::vector<const char*>&& salvageExtensions, VChain<VkPhysicalDeviceFeatures2, VkPhysicalDeviceBufferDeviceAddressFeatures, VkPhysicalDeviceTimelineSemaphoreFeatures, VkPhysicalDeviceSynchronization2Features, VkPhysicalDeviceDynamicRenderingFeatures, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT> const& GIVEN_FEATURES) :
				pPhysicalDevice{ givenPhysicalDevice },
				logicalDeviceInfo{ GIVEN_LOGICAL_DEVICE_INFO },
				queueFamilyInfos{ std::move(salvageQfInfos) },
				queueFamilyPriorities{ std::move(salvageQfPriorities) },
				extensions{ std::move(salvageExtensions) },
				features{ GIVEN_FEATURES } {
				reroutePointers();
			}
			CreateInfo(CreateInfo&& salvageCreateInfo) noexcept : 
				pPhysicalDevice{ salvageCreateInfo.pPhysicalDevice },
				logicalDeviceInfo{ salvageCreateInfo.logicalDeviceInfo },
				queueFamilyInfos(std::move(salvageCreateInfo.queueFamilyInfos)),
				queueFamilyPriorities(std::move(salvageCreateInfo.queueFamilyPriorities)),
				extensions(std::move(salvageCreateInfo.extensions)),
				features{ salvageCreateInfo.features } {
				reroutePointers();
			}
		};

		private:
		Instance* pInstance{};
		VkPhysicalDevice pPhysicalDevice{};
		VkLogicalDevice pLogicalDevice{};
		std::vector<VkQueue> graphicsQueues{};
		const CreateInfo CREATE_INFO;
		const uint32_t GRAPHICS_QF_INDEX{ UINT32_MAX };

		public:		
		explicit Devices(Instance* pGivenInstance, CreateInfo&& givenCreateInfo);
		~Devices();
		[[nodiscard]] Instance*& getInstance() { return pInstance; }
		[[nodiscard]] VkPhysicalDevice& getPhysicalDevice() { return pPhysicalDevice; }
		[[nodiscard]] VkLogicalDevice& getLogicalDevice() { return pLogicalDevice; }
		[[nodiscard]] std::vector<VkQueue>& getGraphicsQueues() { return graphicsQueues; }
		[[nodiscard]] CreateInfo const& getCreateInfo() { return CREATE_INFO; }
		[[nodiscard]] uint32_t const& getGraphicsQfIndex() { return GRAPHICS_QF_INDEX; }

		DELETE_COPY_CONSTRUCTORS(Devices)
		DELETE_MOVE_CONSTRUCTORS(Devices)
	};
}