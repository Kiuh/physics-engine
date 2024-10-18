#pragma once

#include "glm/glm.hpp"
#include <vector>

struct Contact
{
	glm::vec2 pos;
	glm::vec2 normal;
	float penetration;
};

struct Collision
{
	Contact contact;
};