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
	bool isParallel(glm::vec2 vec1, glm::vec2 vec2);
	glm::vec2 rotate90(glm::vec2 vec);
	bool in_range(glm::vec2 val, glm::vec2 a, glm::vec2 b);
	bool greater(glm::vec2 source, glm::vec2 comparison);
	bool lesser(glm::vec2 source, glm::vec2 comparison);
	glm::vec2 triple_product(const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);
}
