#pragma once

#include "glm/glm.hpp"
#include <vector>

struct Collision
{
	glm::vec2 pos{ 0,0 };
	glm::vec2 normal{ 0,0 };
	float penetration = 0;
};