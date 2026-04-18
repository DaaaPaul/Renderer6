#include <vulkan/vulkan_core.h>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cstdint>
#include <string>
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "Instance.h"
#include "Util.h"

namespace Backend {
	namespace PhysicalDevice {
		void init() {
			enumerateSystemPhysicalDevices();
			selectPhysicalDevice();
		}
	
		void selectPhysicalDevice() {
			for(int i = 0; i < gSystemPhysicalDevices.size() && !gPhysicalDevice; i++) {
				if(physicalDeviceGood(gSystemPhysicalDeviceProperties[i], gSystemPhysicalDevices[i])) {
					gPhysicalDevice = gSystemPhysicalDevices[i];

					VkPhysicalDeviceProperties physicalDeviceProperties{};
					vkGetPhysicalDeviceProperties(gPhysicalDevice, &physicalDeviceProperties);

					gLimits = physicalDeviceProperties.limits;
				}
			}

			if(!gPhysicalDevice) {
				throw std::runtime_error("There are no suitable GPUs on your system for this to run on");
			}
		}

		void enumerateSystemPhysicalDevices() {
			uint32_t physicalDeviceCount{};
			CHECK_VK_SUCCESS(vkEnumeratePhysicalDevices(Instance::gInstance, &physicalDeviceCount, nullptr), "Failed to enumerate physical devices on your instance");
			gSystemPhysicalDevices.resize(physicalDeviceCount, {});
			gSystemPhysicalDeviceProperties.resize(physicalDeviceCount, {});
			CHECK_VK_SUCCESS(vkEnumeratePhysicalDevices(Instance::gInstance, &physicalDeviceCount, gSystemPhysicalDevices.data()), "Failed to enumerate physical devices on your instance");
			
			for(int i = 0; i < physicalDeviceCount; i++) {
				vkGetPhysicalDeviceProperties(gSystemPhysicalDevices[i], &gSystemPhysicalDeviceProperties[i]);
			}
		}

		bool physicalDeviceGood(VkPhysicalDeviceProperties const& PROPERTIES, VkPhysicalDevice physicalDevice) {
			return apiVersionCheck(PROPERTIES) && extensionsCheck(physicalDevice) && featuresCheck(physicalDevice) && queuesCheck(physicalDevice);
		}

		bool apiVersionCheck(VkPhysicalDeviceProperties const& PROPERTIES) {
			return PROPERTIES.apiVersion >= VK_API_VERSION_1_3;
		}

		bool extensionsCheck(VkPhysicalDevice physicalDevice) {
			uint32_t extensionsCount{};
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionsCount, availableExtensions.data());

			std::vector<std::string> availableExtensionsNames{};
			for(VkExtensionProperties const& AVAILABLE : availableExtensions) {
				availableExtensionsNames.emplace_back(AVAILABLE.extensionName);
			}

			return Util::containsAll(availableExtensionsNames, Util::constCharToString(LogicalDevice::gExtensions));
		}

		bool featuresCheck(VkPhysicalDevice physicalDevice) {
			auto availableFeatures(LogicalDevice::gFeatures);
			vkGetPhysicalDeviceFeatures2(physicalDevice, &availableFeatures.feature);

			return availableFeatures.hasAllOf(LogicalDevice::gFeatures);
		}

		bool queuesCheck(VkPhysicalDevice physicalDevice) {
			using namespace LogicalDevice;
			bool hasAllQueueFamiliesWithEnoughQueues = true;

			uint32_t availableQfCount = UINT32_MAX;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &availableQfCount, nullptr);
			std::vector<VkQueueFamilyProperties> availableQfs(availableQfCount, {});
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &availableQfCount, availableQfs.data());

			bool thisQfGood = false;
			for(int i = 0; i < gQUEUE_FAMILY_CAPABILITIES.size() && hasAllQueueFamiliesWithEnoughQueues; i++) {
				thisQfGood = false;

				for(int j = 0; j < availableQfs.size() && !thisQfGood; j++) {
					if ((availableQfs[j].queueFlags & gQUEUE_FAMILY_CAPABILITIES[i]) && 
					    (availableQfs[j].queueCount >= gQUEUES_PER_QUEUE_FAMILY[i])) {
						thisQfGood = true;
						gQueueFamilyIndices[i] = j;
					}
				}

				if(!thisQfGood) {
					hasAllQueueFamiliesWithEnoughQueues = false;
				}
			}

			return hasAllQueueFamiliesWithEnoughQueues;
		}
	}
}