#include "random_helper.h"

float randomFloat(float a, float b)
{
	return glm::linearRand(a, b);
}

float randomFloat01()
{
	return glm::linearRand(0.0f, 1.0f);
}