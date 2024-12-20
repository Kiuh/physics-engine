#pragma once

#include "glm/glm.hpp"
#include "imgui.h"
#include "random_helper.h"

struct Color
{
	glm::vec3 value;

	Color();
	Color(glm::vec3 value);

	glm::vec3 getValue() const;
	ImU32 getImGuiColor() const;

	static Color red();
	static Color green();
	static Color blue();

	static Color yellow();

	static Color white();
	static Color black();

	static Color randomColor();
};
