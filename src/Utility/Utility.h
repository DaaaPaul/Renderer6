#pragma once

#include <cmath>
#include <glm/glm.hpp>
#include <iostream>
#include <string>
#include <vector>

#define PRINTLN(x) std::cout << x << '\n'

#define THROW_RUNTIME(x) throw std::runtime_error(x)

#define UINT32(x) static_cast<uint32_t>(x)

#define DA_PI 3.14159274f

#define CHECK_NULLPTR(p, error_msg) \
	if(!p) { \
		throw std::runtime_error(error_msg); \
	}

inline std::ostream& operator<<(std::ostream& os, glm::vec2 const& vec2) {
	return os << '[' << vec2.x << ", " << vec2.y << ']';
}

inline std::ostream& operator<<(std::ostream& os, glm::vec3 const& vec3) {
	return os << '[' << vec3.x << ", " << vec3.y << ", " << vec3.z << ']';
}

inline std::ostream& operator<<(std::ostream& os, glm::vec4 const& vec4) {
	return os << '[' << vec4.x << ", " << vec4.y << ", " << vec4.z << ", " << vec4.w << ']';
}

inline std::ostream& operator<<(std::ostream& os, glm::mat4 const& mat4) {
	return os << 
		mat4[0] << '\n' <<
		mat4[1] << '\n' <<
		mat4[2] << '\n' <<
		mat4[3];
}

namespace Utility {
	std::vector<std::string> to_string(const std::vector<const char*>& c_strings);

	bool contains_all(std::vector<std::string> these_strings, std::vector<std::string> contain_these);

	float random();

	std::vector<char> get_file_bytes(const std::string& file_path);

	inline glm::vec2 get_circle_position(float angle, float radius) noexcept {
		return glm::vec2(std::cos(angle) * radius, std::sin(angle) * radius);
	}

	void generate_sphere(std::vector<glm::vec3>& vertices, std::vector<uint32_t>& indices, const float RADIUS, const uint32_t STACK_COUNT, const uint32_t SECTOR_COUNT) noexcept;
}