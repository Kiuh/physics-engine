#pragma once

#include <stdexcept>
#include <string>
#include <vulkan/vk_enum_string_helper.h>
#include <vulkan/vulkan_core.h>

struct GraphicsEngineConfig
{
	bool validation = false;
	bool logging = true;
	int maxFramesInFlight = 2;
};

void VK_CHECK(VkResult res);