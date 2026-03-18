#include <vulkan/vulkan.h>
#include "LogicalDevice.h"

namespace Backend {
	namespace PhysicalDevice {
		inline VkPhysicalDevice gpPhysicalDevice{};
		inline std::vector<VkPhysicalDevice> gSystemPhysicalDevices{};
		inline std::vector<VkPhysicalDeviceProperties> gSystemPhysicalDeviceProperties{};

		void init();
	
		void enumerateSystemPhysicalDevices(std::vector<VkPhysicalDeviceProperties>&, std::vector<VkPhysicalDevice>&);
		void selectPhysicalDevice();

		bool physicalDeviceGood(VkPhysicalDeviceProperties const&, VkPhysicalDevice&);
		bool apiVersionCheck(VkPhysicalDeviceProperties const&);
		bool extensionsCheck(VkPhysicalDevice&);
		bool featuresCheck(VkPhysicalDevice&);
		bool queueCheck(VkPhysicalDeviceProperties const&, VkPhysicalDevice&);
	}
}