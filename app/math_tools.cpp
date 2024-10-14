#pragma once

#include <glm/gtc/epsilon.hpp>
#include <glm/ext/scalar_constants.hpp>

static inline bool is_eq_f(float f1, float f2)
{
	return glm::epsilonEqual(f1, f2, glm::epsilon<float>());
}

static inline bool is0_f(float f)
{
	return is_eq_f(f, 0.0f);
}