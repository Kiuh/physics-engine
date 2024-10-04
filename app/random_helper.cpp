#pragma once

#include <iostream>
#include <random>

static float randomFloat(float a, float b)
{
	std::random_device rd;
	std::mt19937 gen = std::mt19937(rd());
	std::uniform_real_distribution<> dis(a, b);
	return static_cast<float>(dis(gen));
}

static float randomFloat01()
{
	return randomFloat(0, 1);
}