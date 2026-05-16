#pragma once

#include "Util.h"

namespace Memory  {
	namespace Host {
		inline VkDeviceMemory gMemory{};
		inline std::vector<Util::Memory::BufferBundle> gBuffers{};

		inline std::vector<VkBufferCreateInfo> gBufferCreates{};
		inline std::vector<VkMemoryRequirements> gBufferMemoryRequirements{};

		inline std::vector<Util::Memory::ItemType> gMemoryItemTypes{};

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