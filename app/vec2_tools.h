#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/glm.hpp"
#include "math_tools.h"
#include <algorithm>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace vt
{
	float cross_vec2(const glm::vec2& o, const glm::vec2& a, const glm::vec2& b);

	void rotateVec2(glm::vec2& v, float angle);

	// Computes triple product - cross(cross(a, b), c)
	glm::vec2 triple_product(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

	struct SupportResult
	{
		size_t index;
		glm::vec2 point;
	};

	// Gives furthest point among given in given direction
	SupportResult getSupport(const std::vector<glm::vec2>& dots, const glm::vec2& dir);

	// Calculates barycentric coordinates of point corresponding to given triangle
	glm::vec3 calcBarycentricCoords(const glm::vec2& P, const glm::vec2& A, const glm::vec2& B, const glm::vec2& C);

	std::vector<glm::vec2> createConvexHull(const std::vector<glm::vec2>& _points);
}
