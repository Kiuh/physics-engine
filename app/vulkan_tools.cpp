#include "vulkan_tools.h"

void VK_CHECK(VkResult res)
{
	if (res != VK_SUCCESS)
	{
		std::string msg = "Fatal:\nVkResult is " + std::string(string_VkResult(res));
		throw std::runtime_error(msg);
	}
}