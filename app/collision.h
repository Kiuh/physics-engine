#pragma once

#include "glm/glm.hpp"

struct Collision
{
	glm::vec2 normal{ 0,0 };
	float penetration = 0;
};
