#pragma once

#include "glm/glm.hpp"
#include "random_helper.h"

struct Color
{
	glm::vec3 value;

	Color();
	Color(glm::vec3 value);

	glm::vec3 getValue() const;

	static Color red();
	static Color white();
};

Color randomColor();