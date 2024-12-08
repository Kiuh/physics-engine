#pragma once

#include <glm/glm.hpp>
#include <vector>

struct Collision
{
	glm::vec2 normalAB{ 0,0 };
	glm::vec2 normalBA{ 0,0 };
	float penetration = 0;

	glm::vec2 localContactPointA;
	glm::vec2 localContactPointB;

	glm::vec2 worldContactPointA;
	glm::vec2 worldContactPointB;

	std::vector<glm::vec2> gizmo_minkConvex;
	std::vector<glm::vec2> gizmo_minkTangent;
	std::vector<glm::vec2> gizmo_finalTriangle;

	glm::vec2 getNormalVecAB() const {
		return normalAB * penetration;
	}

	glm::vec2 getNormalVecBA() const {
		return normalBA * penetration;
	}
};
