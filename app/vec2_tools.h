#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/glm.hpp"
#include "math_tools.h"
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

namespace vt
{
	void rotateVec2(glm::vec2& v, float angle);

	// Computes triple product - cross(cross(a, b), c)
	glm::vec2 triple_product(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);

	// Gives furthest point among given in given direction
	std::pair<size_t, glm::vec2> getSupport(const std::vector<glm::vec2>& dots, const glm::vec2& dir);

	// Calculates barycentric coordinates of point corresponding to given triangle
	glm::vec3 calcBarycentricCoords(const glm::vec2& P, const glm::vec2& A, const glm::vec2& B, const glm::vec2& C);
}
