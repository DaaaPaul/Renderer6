#pragma once

#include "Util.h"
#include "MemoryUtil.h"

namespace Memory  {
	namespace Host {
		inline VkDeviceMemory gpMemory{};
		inline std::vector<BufferBundle> gBuffers{};

		inline std::vector<VkBufferCreateInfo> gBufferCreates{};
		inline std::vector<VkMemoryRequirements> gBufferMemoryRequirements{};

		void init();
		void deInit();

		void populateBufferCreates() noexcept;
		void createBuffers();
		void populateBufferMemoryRequirements() noexcept;
		void createMemory();
		void bindBuffers();
		void populateBufferAddresses() noexcept;
		void initializeBufferData() noexcept;

		namespace Util {
			void writeToBuffer(uint32_t const&, void const*, uint32_t const&);
		}
	}
}