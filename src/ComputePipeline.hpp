#pragma once

#include <vulkan/vulkan.h>
#include "Devices.hpp"

namespace Engine {
	class ComputePipeline {
		public:
		struct CreateInfo {
			VkComputePipelineCreateInfo pipelineInfo{};

			CreateInfo(VkComputePipelineCreateInfo const& info) noexcept :
				pipelineInfo(info) {

			}
			CreateInfo(CreateInfo&& salvageCreateInfo) noexcept : 
				pipelineInfo(salvageCreateInfo.pipelineInfo) {

			}
		};

		private:
		Backend::Devices* pDevices{};
		VkPipeline pComputePipeline{};
		const CreateInfo CREATE_INFO;

		public:
		explicit ComputePipeline(Backend::Devices* pDevices, CreateInfo&& givenCreateInfo);
		~ComputePipeline();
		[[nodiscard]] Backend::Devices*& getDevices() noexcept { return pDevices; }
		[[nodiscard]] VkPipeline& getComputePipeline() noexcept { return pComputePipeline; }
		[[nodiscard]] CreateInfo const& getCreateInfo() const noexcept { return CREATE_INFO; }

		DELETE_COPY_CONSTRUCTORS(ComputePipeline)
		DELETE_MOVE_CONSTRUCTORS(ComputePipeline)
	};
}