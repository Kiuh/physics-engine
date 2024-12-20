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

	glm::vec2 worldContactPoint;

	std::vector<glm::vec2> gizmo_minkConvex;
	std::vector<glm::vec2> gizmo_minkTangent;
	std::vector<glm::vec2> gizmo_finalTriangle;
	std::vector<std::vector<glm::vec2>> gizmo_diff_triangles;

	glm::vec2 getNormalVecAB() const {
		return normalAB * penetration;
	}

	glm::vec2 getNormalVecBA() const {
		return normalBA * penetration;
	}
};
