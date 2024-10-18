#pragma once

#include <glm/gtc/random.hpp>

static inline float randomFloat(float a, float b)
{
	return glm::linearRand(a, b);
}

static inline float randomFloat01()
{
	return glm::linearRand(0.0f, 1.0f);
}