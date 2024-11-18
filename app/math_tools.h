#pragma once

#ifndef GLM_ENABLE_EXPERIMENTAL
#define GLM_ENABLE_EXPERIMENTAL
#endif

#include "glm/glm.hpp"
#include <cmath>
#include <glm/ext/scalar_constants.hpp>
#include <glm/gtc/epsilon.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <optional>
#include <stdexcept>
#include <vector>

namespace mt
{
	bool is_eq_f(float f1, float f2);
	bool is0_f(float f);
	float clamp_m11(float f);
	bool in_range(float val, float a, float b);
}