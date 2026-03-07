#pragma once

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <iostream>
#include "Common.h"
#include "Window.hpp"

namespace Backend {
	class Instance {
		public:
		struct CreateInfo {
			std::vector<const char*> layers{};
			std::vector<const char*> extensions{};
			VkApplicationInfo appInfo{};
			VkInstanceCreateInfo createInfo{};

			void reroutePointers() {
				createInfo.pApplicationInfo = &appInfo;
			}
			CreateInfo(std::vector<const char*>&& salvageLayers, std::vector<const char*>&& salvageExtensions, VkApplicationInfo const& GIVEN_APP_INFO, VkInstanceCreateInfo const& GIVEN_INSTANCE_INFO) :
				layers(std::move(salvageLayers)),
				extensions(std::move(salvageExtensions)),
				appInfo(GIVEN_APP_INFO),
				createInfo(GIVEN_INSTANCE_INFO) {
				reroutePointers();
			}
			CreateInfo(CreateInfo&& salvageCreateInfo) : 
				layers(std::move(salvageCreateInfo.layers)),
				extensions(std::move(salvageCreateInfo.extensions)),
				appInfo(salvageCreateInfo.appInfo),
				createInfo(salvageCreateInfo.createInfo) {
				reroutePointers();
			}
		};

		private:
		VkInstance pInstance{};
		Window* pWindow{};
		const CreateInfo CREATE_INFO;
		static void checkHaveExtensions(std::vector<const char*> const& NECESSARY_EXTENSIONS);
		static void checkHaveLayers(std::vector<const char*> const& NECESSARY_LAYERS);

		public:
		explicit Instance(Window* pGivenWindow, CreateInfo&& givenCreateInfo);
		~Instance();
		[[nodiscard]] VkInstance& getInstance() { return pInstance; }
		[[nodiscard]] Window*& getWindow() { return pWindow; }
		[[nodiscard]] CreateInfo const& getCreateInfo() const { return CREATE_INFO; }

		DELETE_COPY_CONSTRUCTORS(Instance)
		DELETE_MOVE_CONSTRUCTORS(Instance)
	};
}