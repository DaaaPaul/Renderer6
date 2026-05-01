#include "Memory.hpp"
#include "PhysicalDevice.h"

namespace Resource {
	Memory::Memory(std::vector<Texture> const& TEXTURES, std::vector<Buffer> const& BUFFERS, std::vector<DescriptorSet> const& SETS, VkMemoryPropertyFlags propertyFlags) {
	
	}

	Memory::~Memory() {
	
	}

	std::pair<VkDeviceSize, std::vector<VkDeviceSize>> Memory::sizeAndOffsets(std::vector<Texture> const& TEXTURES, std::vector<Buffer> const& BUFFER) {
		std::vector<VkDeviceSize> offsets{};

		VkDeviceSize beginning = 0;
		VkDeviceSize ending = 0;
		VkDeviceSize nextOpenSpace = 0;

		for(Texture const& T : TEXTURES) {
			beginning = Util::Memory::alignNextHighest(beginning, )
		}

		for(int i = 0; i < ITEM_REQUIREMENTS.size(); ++i) {
			beginningByte = alignNextHighest(beginningByte, ITEM_REQUIREMENTS[i].alignment);

			if(i > 0) {
				bool linearFollowedByNonLinear = TYPES[i] == ItemType::LINEAR && TYPES[i - 1] == ItemType::NON_LINEAR;
				bool nonLinearFollowedByLinear = TYPES[i] == ItemType::NON_LINEAR && TYPES[i - 1] == ItemType::LINEAR;

				if(linearFollowedByNonLinear || nonLinearFollowedByLinear) {
					nextOpenSpace = alignNextHighest(endingByte, BI_GRANULARITY);

					if(beginningByte < nextOpenSpace) {
						beginningByte = nextOpenSpace;
					}
				}
			}

			beginnings.push_back(beginningByte);
			beginningByte += ITEM_REQUIREMENTS[i].size;
			endingByte = beginningByte - 1;
		}

		return { beginningByte, beginnings };
	}
}