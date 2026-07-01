#include <fstream>
#include <random>	
#include "Utility/Utility.h"

namespace Utility {
	std::vector<std::string> to_string(const std::vector<const char*>& c_strings) {
		std::vector<std::string> strings;

		for(const char* c_string : c_strings) {
			strings.emplace_back(c_string);
		}

		return strings;
	}

	bool contains_all(std::vector<std::string> these_strings, std::vector<std::string> contain_these) {
		if(contain_these.empty()) {
			return true;
		} else {
			std::sort(these_strings.begin(), these_strings.end());
			std::sort(contain_these.begin(), contain_these.end());

			return std::ranges::includes(these_strings, contain_these);
		}
	}

	float random() {
		static std::default_random_engine random_engine(static_cast<unsigned>(time(nullptr)));
		static std::uniform_real_distribution range(0.0f, 1.0f);

		return range(random_engine);
	}

	std::vector<char> get_file_bytes(const std::string& file_path) {
		std::ifstream file_stream_in(file_path, std::ios::binary | std::ios::ate);

		if(!file_stream_in.good()) {
			THROW_RUNTIME("get_file_bytes: Failure reading file at " + file_path);
		}

		uint32_t file_size = file_stream_in.tellg();
		std::vector<char> file_bytes(file_size);

		file_stream_in.seekg(0);

		file_stream_in.read(file_bytes.data(), file_size);

		return file_bytes;
	}
}