#pragma once

#include <glm/glm.hpp>

struct Collision
{
	glm::vec2 normalAB{ 0,0 };
	glm::vec2 normalBA{ 0,0 };
	float penetration = 0;

	glm::vec2 localContactPointA;
	glm::vec2 localContactPointB;

	glm::vec2 worldContactPointA;
	glm::vec2 worldContactPointB;

	glm::vec2 getNormalVecAB() const {
		return normalAB * penetration;
	}

	glm::vec2 getNormalVecBA() const {
		return normalBA * penetration;
	}
};
