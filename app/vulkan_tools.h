#pragma once

#include <magic_enum.hpp>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>

struct GraphicsEngineConfig
{
	bool validation = false;
	bool logging = true;
	int maxFramesInFlight = 2;
};

void VK_CHECK(VkResult res);