#include "PhysicalDevice.h"
#include "Instance.h"

namespace Backend {
	namespace PhysicalDevice {
		void init() {
			enumerateSystemPhysicalDevices(gSystemPhysicalDeviceProperties, gSystemPhysicalDevices);
			selectPhysicalDevice();
		}
	
		void selectPhysicalDevice() {

		}

		void enumerateSystemPhysicalDevices(std::vector<VkPhysicalDeviceProperties>& toSetProperties, std::vector<VkPhysicalDevice>& toSetPds) {
			toSetProperties.clear();
			toSetPds.clear();
			
			uint32_t physicalDeviceCount{};
			CHECK_VK_SUCCESS(vkEnumeratePhysicalDevices(Instance::gpInstance, &physicalDeviceCount, nullptr), "Failed to enumerate physical devices on your instance");
			toSetPds.resize(physicalDeviceCount, {});
			toSetProperties.resize(physicalDeviceCount, {});
			CHECK_VK_SUCCESS(vkEnumeratePhysicalDevices(Instance::gpInstance, &physicalDeviceCount, toSetPds.data()), "Failed to enumerate physical devices on your instance");
			
			for(int i = 0; i < physicalDeviceCount; i++) {
				vkGetPhysicalDeviceProperties(toSetPds[i], &toSetProperties[i]);
			}
		}

		bool physicalDeviceGood(VkPhysicalDeviceProperties const& PROPERTIES, VkPhysicalDevice& pd) {
			return apiVersionCheck(PROPERTIES) && extensionsCheck(pd) && featuresCheck(pd) && queueCheck(PROPERTIES, pd);
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
			auto requestedFeaturesShell = LogicalDevice::gFeatures.getShell(true);
			requestedFeaturesShell.reroutePointers();
			vkGetPhysicalDeviceFeatures2(pd, &requestedFeaturesShell.val);


		}

		bool queueCheck(VkPhysicalDeviceProperties const&, VkPhysicalDevice&) {

		}
	}
}