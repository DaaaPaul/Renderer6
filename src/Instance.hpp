#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include "Common.h"
#include "Window.hpp"

namespace Backend {
	struct Instance {
		struct CreateInfo {
			std::vector<const char*> layers{};
			std::vector<const char*> extensions{};
			VkApplicationInfo appInfo{};
			VkInstanceCreateInfo createInfo{};

			CreateInfo() :
				layers{},
				extensions{},
				appInfo{},
				createInfo{} {}
			CreateInfo(std::vector<const char*> const& GIVEN_LAYERS, std::vector<const char*> const& GIVEN_EXTENSIONS, VkApplicationInfo const& GIVEN_APP_INFO, VkInstanceCreateInfo const& GIVEN_INSTANCE_INFO) :
				layers(GIVEN_LAYERS),
				extensions(GIVEN_EXTENSIONS),
				appInfo(GIVEN_APP_INFO),
				createInfo(GIVEN_INSTANCE_INFO) {}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				layers(std::move(salvageCreateInfo.layers)),
				extensions(std::move(salvageCreateInfo.extensions)),
				appInfo(salvageCreateInfo.appInfo),
				createInfo(salvageCreateInfo.createInfo) {
				createInfo.pApplicationInfo = &appInfo;	
			}
			DELETE_COPY_CONSTRUCTORS(CreateInfo)
		};

		VkInstance instance{};
		Window const* WINDOW{};
		const CreateInfo CREATE_INFO{};

		static void checkHaveExtensions(std::vector<const char*> const& NECESSARY_EXTENSIONS);
		static void checkHaveLayers(std::vector<const char*> const& NECESSARY_LAYERS);

		explicit Instance(Window* givenWindow, CreateInfo&& givenCreateInfo);
		~Instance();

		DELETE_COPY_CONSTRUCTORS(Instance)
		DELETE_MOVE_CONSTRUCTORS(Instance)
	};
}