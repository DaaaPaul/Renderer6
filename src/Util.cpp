#include <algorithm>
#include <fstream>
#include <random>
#include "Util.h"

namespace Util {
    bool containsAll(std::vector<std::string> const& BIG, std::vector<std::string> const& SMALL) {
        std::vector<std::string> bigCopy(BIG);
        std::vector<std::string> smallCopy(SMALL);

        std::sort(bigCopy.begin(), bigCopy.end());
        std::sort(smallCopy.begin(), smallCopy.end());

        return std::ranges::includes(bigCopy, smallCopy);
    }

	std::vector<char> getFileBytes(std::string const& PATH) {
		std::ifstream fileIn(PATH, std::ios::binary | std::ios::ate);
		if(!fileIn.good()) {
			throw std::runtime_error("Failure reading file at " + std::string(PATH));
		}

		uint32_t fileSize = fileIn.tellg();
		std::vector<char> bytes(fileSize);

		fileIn.seekg(0);
		fileIn.read(bytes.data(), fileSize);

		return bytes;
	}

	float random() noexcept {
		static std::default_random_engine gEngine(static_cast<unsigned>(time(nullptr)));
		static std::uniform_real_distribution gRange(0.0f, 1.0f);

		return gRange(gEngine);
	}

	constexpr uint16_t pointersSize(uint16_t const& HOW_MANY) noexcept {
		return HOW_MANY * 8;
	}
}