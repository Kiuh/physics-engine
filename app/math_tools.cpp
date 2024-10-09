#pragma once

#include <limits>
#include <algorithm>
#include <cmath>

static inline bool AreEqual(float f1, float f2)
{
	return (std::fabs(f1 - f2) <= std::numeric_limits<float>::epsilon() * fmax(std::fabs(f1), std::fabs(f2)));
}