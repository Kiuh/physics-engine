#pragma once

#include "glm/glm.hpp"

struct Collision
{
	bool isCollide = false;

	glm::vec2 normal{};
	float depth = 0;
};