#pragma once

#include "Utility/Utility.h"

namespace Memory  {
	namespace Host {
		inline VkDeviceMemory gMemory{};
		inline std::vector<Utility::Memory::BufferBundle> gBuffers{};

		inline std::vector<VkBufferCreateInfo> gBufferCreates{};
		inline std::vector<VkMemoryRequirements> gBufferMemoryRequirements{};

		inline std::vector<Utility::Memory::ItemType> gMemoryItemTypes{};

		void init();
		void destroy();

		void populateBufferCreates();
		void createBuffers();
		void populateBufferMemoryRequirements();
		void createMemory();
		void bind_buffers();
		void populateBufferAddresses();
		void initializeBufferData();

		namespace Mutate {
			void writeToBuffer(uint32_t const&, void const*, uint32_t const&);
		}
	}
}