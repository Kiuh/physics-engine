#include "shader_manager.h"

VkShaderModule ShaderManager::createShaderModule(const std::string& filename)
{
	auto shaderCode = readFile(filename);
	return createShaderModule(shaderCode);
}

VkShaderModule  ShaderManager::createShaderModule(const std::vector<char>& code) const
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	VK_CHECK(vkCreateShaderModule(vulkanDevice->logicalDevice, &createInfo, nullptr, &shaderModule));
	return shaderModule;
}

std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}