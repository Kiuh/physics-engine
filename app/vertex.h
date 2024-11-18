#pragma once

#include "glm/glm.hpp"
#include <array>
#include <cstddef>
#include <vulkan/vulkan_core.h>

struct Vertex
{
	glm::vec2 pos = { 0,0 };
	glm::vec3 color = { 0,0,0 };

	static VkVertexInputBindingDescription getBindingDescription();
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions();
};