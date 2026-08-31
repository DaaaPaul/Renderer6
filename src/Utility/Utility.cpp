#include <fstream>
#include <random>	
#include "utility/Utility.h"

std::ostream& operator<<(std::ostream& os, glm::vec2 vec2) {
	return os << '[' << vec2.x << ", " << vec2.y << ']';
}

std::ostream& operator<<(std::ostream& os, glm::vec3 vec3) {
	return os << '[' << vec3.x << ", " << vec3.y << ", " << vec3.z << ']';
}

std::ostream& operator<<(std::ostream& os, glm::vec4 vec4) {
	return os << '[' << vec4.x << ", " << vec4.y << ", " << vec4.z << ", " << vec4.w << ']';
}

std::ostream& operator<<(std::ostream& os, glm::mat4 mat4) {
	return os << 
		'[' << mat4[0][0] << ", " << mat4[1][0] << ", " << mat4[2][0] << ", " << mat4[3][0] << "]\n" <<
		'[' << mat4[0][1] << ", " << mat4[1][1] << ", " << mat4[2][1] << ", " << mat4[3][1] << "]\n" <<
		'[' << mat4[0][2] << ", " << mat4[1][2] << ", " << mat4[2][2] << ", " << mat4[3][2] << "]\n" <<
		'[' << mat4[0][3] << ", " << mat4[1][3] << ", " << mat4[2][3] << ", " << mat4[3][3] << ']';
}

namespace Utility {
	std::vector<std::string> to_string(const std::vector<const char*>* C_STRINGS) {
		std::vector<std::string> strings;
		strings.reserve(C_STRINGS->size());

		for(int i = 0; i < C_STRINGS->size(); ++i) {
			strings.emplace_back((*C_STRINGS)[i]);
		}

		return strings;
	}

	bool contains_all(std::vector<std::string> these_strings, std::vector<std::string> contain_these) {
		bool contains_all = false;

		if(contain_these.empty()) {
			contains_all = true;
		} else {
			std::sort(these_strings.begin(), these_strings.end());
			std::sort(contain_these.begin(), contain_these.end());

			contains_all = std::ranges::includes(these_strings, contain_these);
		}

		return contains_all;
	}

	float random() {
		static std::default_random_engine random_engine(static_cast<unsigned>(time(nullptr)));
		static std::uniform_real_distribution range(0.0f, 1.0f);

		return range(random_engine);
	}

	std::vector<char> get_file_bytes(std::string file_path) {
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

	std::pair<std::vector<glm::vec3>,  std::vector<uint32_t>> get_sphere(float radius, uint32_t stack_count, uint32_t sector_count) {
		const float SECTOR_STEP = 2 * Utility::PI / sector_count;
		const float STACK_STEP = Utility::PI / stack_count;

		float sector_angle{}, stack_angle{};
		float xy{}, x{}, y{}, z{};

		uint32_t k1{}, k2{};

		std::vector<glm::vec3> vertices;
		std::vector<uint32_t> indices;

		for(int i = 0; i <= stack_count; ++i) {
			stack_angle = Utility::PI / 2 - i * STACK_STEP;
			xy = radius * std::cos(stack_angle);
			z = radius * std::sin(stack_angle);

			if(i < stack_count) {
				k1 = i * (sector_count + 1);
				k2 = k1 + sector_count + 1;
			}

			for(int j = 0; j <= sector_count; ++j, ++k1, ++k2) {
				sector_angle = j * SECTOR_STEP;

				x = xy * std::cos(sector_angle);
				y = xy * std::sin(sector_angle);

				vertices.emplace_back(x, y, z);

				if(i < stack_count && j < sector_count) {
					if(i != 0) {
						indices.push_back(k1);
						indices.push_back(k2);
						indices.push_back(k1 + 1);
					}

					if(i != (stack_count - 1)) {
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