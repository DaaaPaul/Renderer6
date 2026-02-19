#pragma once

#include <vulkan/vulkan.h>
#include "Common.h"
#include "Instance.hpp"

namespace Backend {
	class Devices {
		public:
		struct CreateInfo {
			VkPhysicalDevice physicalDevice{};
			VkDeviceCreateInfo logicalDeviceInfo{};
			std::vector<VkDeviceQueueCreateInfo> queueFamilyInfos{};
			std::vector<std::vector<float>> queueFamilyPriorities{};
			std::vector<const char*> extensions{};
			VkPhysicalDeviceExtendedDynamicState2FeaturesEXT extendedDynamicStateFeatures{};
			VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeatures{};
			VkPhysicalDeviceSynchronization2Features sync2Features{};
			VkPhysicalDeviceFeatures2 features{};

			void reroutePointers() {
				logicalDeviceInfo.pNext = &features;
				features.pNext = &sync2Features;
				sync2Features.pNext = &dynamicRenderingFeatures;
				dynamicRenderingFeatures.pNext = &extendedDynamicStateFeatures;
			}
			CreateInfo() :
				physicalDevice{},
				logicalDeviceInfo{},
				queueFamilyInfos{},
				queueFamilyPriorities{},
				extensions{},
				extendedDynamicStateFeatures{},
				dynamicRenderingFeatures{},
				sync2Features{},
				features{} {}
			CreateInfo(VkPhysicalDevice&& givenPhysicalDevice, VkDeviceCreateInfo const& GIVEN_LOGICAL_DEVICE_INFO, std::vector<VkDeviceQueueCreateInfo> const& GIVEN_QF_INFOS, std::vector<std::vector<float>> const& GIVEN_QF_PRIORITIES, std::vector<const char*> const& GIVEN_EXTENSIONS, VkPhysicalDeviceExtendedDynamicState2FeaturesEXT const& GIVEN_EXTENDED_DYNAMIC, VkPhysicalDeviceDynamicRenderingFeatures const& GIVEN_DYNAMIC_RENDERING, VkPhysicalDeviceSynchronization2Features const& GIVEN_SYNC2, VkPhysicalDeviceFeatures2 const& GIVEN_FEATURES) :
				physicalDevice{ givenPhysicalDevice },
				logicalDeviceInfo{ GIVEN_LOGICAL_DEVICE_INFO },
				queueFamilyInfos{ GIVEN_QF_INFOS },
				queueFamilyPriorities{ GIVEN_QF_PRIORITIES },
				extensions{ GIVEN_EXTENSIONS },
				extendedDynamicStateFeatures{ GIVEN_EXTENDED_DYNAMIC },
				dynamicRenderingFeatures{ GIVEN_DYNAMIC_RENDERING },
				sync2Features{ GIVEN_SYNC2 },
				features{ GIVEN_FEATURES } {
				reroutePointers();
			}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				physicalDevice{ salvageCreateInfo.physicalDevice },
				logicalDeviceInfo{ salvageCreateInfo.logicalDeviceInfo },
				queueFamilyInfos(std::move(salvageCreateInfo.queueFamilyInfos)),
				queueFamilyPriorities(std::move(salvageCreateInfo.queueFamilyPriorities)),
				extensions(std::move(salvageCreateInfo.extensions)),
				extendedDynamicStateFeatures{ salvageCreateInfo.extendedDynamicStateFeatures },
				dynamicRenderingFeatures{ salvageCreateInfo.dynamicRenderingFeatures },
				sync2Features{ salvageCreateInfo.sync2Features },
				features{ salvageCreateInfo.features } {
				reroutePointers();
			}
			DELETE_COPY_CONSTRUCTORS(CreateInfo)
		};

		private:
		Instance* instance{};
		VkPhysicalDevice physicalDevice{};
		VkLogicalDevice logicalDevice{};
		std::vector<VkQueue> graphicsQueues{};
		const CreateInfo CREATE_INFO{};
		const uint32_t GRAPHICS_QF_INDEX{ UINT32_MAX };

		public:		
		explicit Devices(Instance* givenInstance, CreateInfo&& givenCreateInfo);
		~Devices();
		[[nodiscard]] Instance*& getInstance() { return instance; }
		[[nodiscard]] VkPhysicalDevice& getPhysicalDevice() { return physicalDevice; }
		[[nodiscard]] VkLogicalDevice& getLogicalDevice() { return logicalDevice; }
		[[nodiscard]] std::vector<VkQueue>& getGraphicsQueues() { return graphicsQueues; }
		[[nodiscard]] CreateInfo const& getCreateInfo() { return CREATE_INFO; }
		[[nodiscard]] uint32_t const& getGraphicsQfIndex() { return GRAPHICS_QF_INDEX; }

		DELETE_COPY_CONSTRUCTORS(Devices)
		DELETE_MOVE_CONSTRUCTORS(Devices)
	};
}