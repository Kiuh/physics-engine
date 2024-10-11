#pragma once

#include "random_helper.cpp"
#include "glm/glm.hpp"

struct Color
{
	glm::vec3 value;

	Color()
	{
		this->value = glm::vec3(1, 1, 1);
	}

	Color(glm::vec3 value)
	{
		this->value = value;
	}

	glm::vec3 getValue() const
	{
		return value;
	}

	static Color randomColor()
	{
		auto values = glm::vec3{
			randomFloat01(),
			randomFloat01(),
			randomFloat01(),
		};
		return Color(values);
	}
};