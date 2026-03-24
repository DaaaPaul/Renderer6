#include "PhysicalDevice.h"
#include "Instance.h"

namespace Backend {
	namespace PhysicalDevice {
		void init() {
			enumerateSystemPhysicalDevices();
			selectPhysicalDevice();
		}
	
		void selectPhysicalDevice() {
			for(int i = 0; i < gSystemPhysicalDevices.size() && !gpPhysicalDevice; i++) {
				if(physicalDeviceGood(gSystemPhysicalDeviceProperties[i], gSystemPhysicalDevices[i])) {
					gpPhysicalDevice = gSystemPhysicalDevices[i];

					VkPhysicalDeviceProperties pdProperties{};
					vkGetPhysicalDeviceProperties(gpPhysicalDevice, &pdProperties);

					gBufferImageGranularity = pdProperties.limits.bufferImageGranularity;
				}
			}

			if(!gpPhysicalDevice) {
				throw std::runtime_error("There are no suitable GPUs on your system for this to run on");
			}
		}

		void enumerateSystemPhysicalDevices() {
			uint32_t physicalDeviceCount{};
			CHECK_VK_SUCCESS(vkEnumeratePhysicalDevices(Instance::gpInstance, &physicalDeviceCount, nullptr), "Failed to enumerate physical devices on your instance");
			gSystemPhysicalDevices.resize(physicalDeviceCount, {});
			gSystemPhysicalDeviceProperties.resize(physicalDeviceCount, {});
			CHECK_VK_SUCCESS(vkEnumeratePhysicalDevices(Instance::gpInstance, &physicalDeviceCount, gSystemPhysicalDevices.data()), "Failed to enumerate physical devices on your instance");
			
			for(int i = 0; i < physicalDeviceCount; i++) {
				vkGetPhysicalDeviceProperties(gSystemPhysicalDevices[i], &gSystemPhysicalDeviceProperties[i]);
			}
		}

		bool physicalDeviceGood(VkPhysicalDeviceProperties const& PROPERTIES, VkPhysicalDevice& pd) {
			return apiVersionCheck(PROPERTIES) && extensionsCheck(pd) && featuresCheck(pd) && queuesCheck(pd);
		}

		bool apiVersionCheck(VkPhysicalDeviceProperties const& PROPERTIES) {
			return PROPERTIES.apiVersion >= VK_API_VERSION_1_3;
		}

		bool extensionsCheck(VkPhysicalDevice& pd) {
			uint32_t extensionsCount{};
			vkEnumerateDeviceExtensionProperties(pd, nullptr, &extensionsCount, nullptr);
			std::vector<VkExtensionProperties> availableExtensions(extensionsCount);
			vkEnumerateDeviceExtensionProperties(pd, nullptr, &extensionsCount, availableExtensions.data());

			std::vector<std::string> availableExtensionsNames{};
			for(VkExtensionProperties const& AVAILABLE : availableExtensions) {
				availableExtensionsNames.emplace_back(AVAILABLE.extensionName);
			}

			return Util::containsAll(availableExtensionsNames, Util::constCharToString(LogicalDevice::gExtensions));
		}

		bool featuresCheck(VkPhysicalDevice& pd) {
			auto availableFeatures = LogicalDevice::gFeatures.getShell(true);
			availableFeatures.reroutePointers();
			vkGetPhysicalDeviceFeatures2(pd, &availableFeatures.feature);

			return LogicalDevice::gFeatures.hasAllOf(availableFeatures);
		}

		bool queuesCheck(VkPhysicalDevice& pd) {
			using namespace LogicalDevice;
			bool hasAllQueueFamiliesWithEnoughQueues = true;

			uint32_t availableQfCount = UINT32_MAX;
			vkGetPhysicalDeviceQueueFamilyProperties(pd, &availableQfCount, nullptr);
			std::vector<VkQueueFamilyProperties> availableQfs(availableQfCount, {});
			vkGetPhysicalDeviceQueueFamilyProperties(pd, &availableQfCount, availableQfs.data());

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