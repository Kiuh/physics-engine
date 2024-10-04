#pragma once

#include "random_helper.cpp"
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