#pragma once

#include <glm.hpp>

struct Color
{
	glm::vec3 values;

	Color()
	{
		this->values = glm::vec3(1, 1, 1);
	}

	Color(glm::vec3 values)
	{
		this->values = values;
	}

	glm::vec3 getValues() const
	{
		return values;
	}
};