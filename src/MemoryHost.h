#pragma once

#include "Util.h"

namespace Memory  {
	namespace Host {
		inline VkDeviceMemory gpMemory{};
		inline std::vector<Util::Memory::BufferBundle> gBuffers{};

		inline std::vector<VkBufferCreateInfo> gBufferCreates{};
		inline std::vector<VkMemoryRequirements> gBufferMemoryRequirements{};

		inline std::vector<Util::Memory::ItemType> gMemoryItemTypes{};

		void init();
		void deInit();

		void populateBufferCreates() noexcept;
		void createBuffers();
		void populateBufferMemoryRequirements() noexcept;
		void createMemory();
		void bindBuffers();
		void populateBufferAddresses() noexcept;
		void initializeBufferData() noexcept;

		namespace Mutate {
			void writeToBuffer(uint32_t const&, void const*, uint32_t const&);
		}
	}
}