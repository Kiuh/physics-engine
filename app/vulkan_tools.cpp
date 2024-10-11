#pragma once

#include <magic_enum/magic_enum.hpp>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>

struct GraphicsEngineConfig
{
	bool isDebug = false;
	int maxFramesInFlight = 2;
};

static void VK_CHECK(VkResult res)
{
	if (res != VK_SUCCESS)
	{
		std::string msg = "Fatal:\nVkResult is " + std::string(magic_enum::enum_name((res)));
		throw std::runtime_error(msg);
	}
}