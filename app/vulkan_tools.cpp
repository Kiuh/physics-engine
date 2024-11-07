#include "vulkan_tools.h"

void VK_CHECK(VkResult res)
{
	if (res != VK_SUCCESS)
	{
		std::string msg = "Fatal:\nVkResult is " + std::string(magic_enum::enum_name((res)));
		throw std::runtime_error(msg);
	}
}