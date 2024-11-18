#include "math_tools.h"

namespace mt
{
	bool is_eq_f(float f1, float f2)
	{
		return glm::epsilonEqual(f1, f2, glm::epsilon<float>());
	}

	bool is0_f(float f)
	{
		return is_eq_f(f, 0.0f);
	}

	float clamp_m11(float f)
	{
		return glm::clamp(f, -1.0f, 1.0f);
	}

	bool in_range(float val, float a, float b)
	{
		return val >= fmin(a, b) - glm::epsilon<float>() && val <= fmax(a, b) + glm::epsilon<float>();
	}
}