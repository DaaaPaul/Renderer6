#include <fstream>
#include <random>	
#include "utility/Utility.h"

std::ostream& operator<<(std::ostream& os, glm::vec2 const& vec2) {
	return os << '[' << vec2.x << ", " << vec2.y << ']';
}

std::ostream& operator<<(std::ostream& os, glm::vec3 const& vec3) {
	return os << '[' << vec3.x << ", " << vec3.y << ", " << vec3.z << ']';
}

std::ostream& operator<<(std::ostream& os, glm::vec4 const& vec4) {
	return os << '[' << vec4.x << ", " << vec4.y << ", " << vec4.z << ", " << vec4.w << ']';
}

std::ostream& operator<<(std::ostream& os, glm::mat4 const& mat4) {
	return os << 
		mat4[0] << '\n' <<
		mat4[1] << '\n' <<
		mat4[2] << '\n' <<
		mat4[3];
}

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
			throw std::runtime_error("get_file_bytes: Failure reading file at " + file_path);
		}

		uint32_t file_size = file_stream_in.tellg();
		std::vector<char> file_bytes(file_size);

		file_stream_in.seekg(0);

		file_stream_in.read(file_bytes.data(), file_size);

		return file_bytes;
	}

	glm::vec2 get_circle_position(float angle, float radius) {
		return glm::vec2(std::cos(angle) * radius, std::sin(angle) * radius);
	}

	std::pair<std::vector<glm::vec3>,  std::vector<uint32_t>> get_sphere(const float RADIUS, const uint32_t STACK_COUNT, const uint32_t SECTOR_COUNT) {
		const float SECTOR_STEP = 2 * Utility::PI / SECTOR_COUNT;
		const float STACK_STEP = Utility::PI / STACK_COUNT;

		float sector_angle{}, stack_angle{};
		float xy{}, x{}, y{}, z{};

		uint32_t k1{}, k2{};

		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;

		for(int i = 0; i <= STACK_COUNT; ++i) {
			stack_angle = Utility::PI / 2 - i * STACK_STEP;
			xy = RADIUS * std::cos(stack_angle);
			z = RADIUS * std::sin(stack_angle);

			if(i < STACK_COUNT) {
				k1 = i * (SECTOR_COUNT + 1);
				k2 = k1 + SECTOR_COUNT + 1;
			}

			for(int j = 0; j <= SECTOR_COUNT; ++j, ++k1, ++k2) {
				sector_angle = j * SECTOR_STEP;

				x = xy * std::cos(sector_angle);
				y = xy * std::sin(sector_angle);

				vertices.emplace_back(x, y, z);

				if(i < STACK_COUNT && j < SECTOR_COUNT) {
					if(i != 0) {
						indices.push_back(k1);
						indices.push_back(k2);
						indices.push_back(k1 + 1);
					}

					if(i != (STACK_COUNT - 1)) {
						indices.push_back(k1 + 1);
						indices.push_back(k2);
						indices.push_back(k2 + 1);
					}
				}
			}
		}

		return { std::move(vertices), std::move(indices) };
	}
}