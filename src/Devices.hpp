#pragma once

#include "Common.h"
#include "Instance.hpp"

namespace Backend {
	class Devices {
		public:
		struct CreateInfo {
			VkPhysicalDevice pPhysicalDevice{};
			VkDeviceCreateInfo logicalDeviceInfo{};
			std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos{};
			std::vector<std::vector<float>> queueFamilyPriorities{};
			std::vector<const char*> extensions{};
			VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extendedDynamicStateFeatures{};
			VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
			VkPhysicalDeviceSynchronization2Features sync2Features{};
			VkPhysicalDeviceTimelineSemaphoreFeatures timelineFeatures{};
			VkPhysicalDeviceFeatures2 features{};

			void reroutePointers() {
				logicalDeviceInfo.pNext = &features;
				features.pNext = &timelineFeatures;
				timelineFeatures.pNext = &sync2Features;
				sync2Features.pNext = &dynamicRenderingFeatures;
				dynamicRenderingFeatures.pNext = &extendedDynamicStateFeatures;
			}
			CreateInfo(VkPhysicalDevice&& givenPhysicalDevice, VkDeviceCreateInfo const& GIVEN_LOGICAL_DEVICE_INFO, std::vector<VkDeviceQueueCreateInfo>&& salvageQfInfos, std::vector<std::vector<float>>&& salvageQfPriorities, std::vector<const char*>&& salvageExtensions, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT const& GIVEN_EXTENDED_DYNAMIC, VkPhysicalDeviceDynamicRenderingFeatures const& GIVEN_DYNAMIC_RENDERING, VkPhysicalDeviceSynchronization2Features const& GIVEN_SYNC2, VkPhysicalDeviceTimelineSemaphoreFeatures const& GIVEN_TIMELINE, VkPhysicalDeviceFeatures2 const& GIVEN_FEATURES) :
				pPhysicalDevice{ givenPhysicalDevice },
				logicalDeviceInfo{ GIVEN_LOGICAL_DEVICE_INFO },
				queueFamilyInfos{ std::move(salvageQfInfos) },
				queueFamilyPriorities{ std::move(salvageQfPriorities) },
				extensions{ std::move(salvageExtensions) },
				extendedDynamicStateFeatures{ GIVEN_EXTENDED_DYNAMIC },
				dynamicRenderingFeatures{ GIVEN_DYNAMIC_RENDERING },
				sync2Features{ GIVEN_SYNC2 },
				timelineFeatures{ GIVEN_TIMELINE },
				features{ GIVEN_FEATURES } {
				reroutePointers();
			}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				pPhysicalDevice{ salvageCreateInfo.pPhysicalDevice },
				logicalDeviceInfo{ salvageCreateInfo.logicalDeviceInfo },
				queueFamilyInfos(std::move(salvageCreateInfo.queueFamilyInfos)),
				queueFamilyPriorities(std::move(salvageCreateInfo.queueFamilyPriorities)),
				extensions(std::move(salvageCreateInfo.extensions)),
				extendedDynamicStateFeatures{ salvageCreateInfo.extendedDynamicStateFeatures },
				dynamicRenderingFeatures{ salvageCreateInfo.dynamicRenderingFeatures },
				sync2Features{ salvageCreateInfo.sync2Features },
				timelineFeatures{ salvageCreateInfo.timelineFeatures },
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