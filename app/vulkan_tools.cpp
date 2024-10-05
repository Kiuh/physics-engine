#pragma once

#include <cstring>
#include <iostream>
#include <magic_enum/magic_enum.hpp>
#include <stdexcept>
#include <string>
#include <vulkan/vulkan_core.h>

#define VK_CHECK(f)													            \
{																			    \
	VkResult res = (f);														    \
	if (res != VK_SUCCESS)													    \
	{																		    \
		std::string msg = "Fatal: VkResult is \"" + vk_errorString(res) + "\""; \
		throw std::runtime_error(msg);                                          \
	}																		    \
}                                                                               \

static std::string vk_errorString(VkResult errorCode)
{
	return std::string(magic_enum::enum_name(errorCode));
}