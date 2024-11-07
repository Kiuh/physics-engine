#pragma once

#include "vulkan_device.h"
#include "vulkan_tools.h"
#include <fstream>
#include <iosfwd>
#include <iostream>
#include <vector>
#include <vulkan/vulkan_core.h>

std::vector<char> readFile(const std::string& filename);

struct ShaderManager
{
	public:
	// Required
	VulkanDevice* vulkanDevice;

	VkShaderModule createShaderModule(const std::string& filename);

	private:
	VkShaderModule createShaderModule(const std::vector<char>& code) const;
};