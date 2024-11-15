#pragma once

#include "random_helper.h"
#include "glm/glm.hpp"

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