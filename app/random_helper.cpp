#pragma once

#include <iostream>
#include <random>

class RandomHelper
{
	private:
	std::random_device rd;
	std::mt19937 gen;

	public:
	RandomHelper()
	{
		gen = std::mt19937(rd());
	}

	float randomFloat(float a, float b)
	{
		std::uniform_real_distribution<> dis(a, b);
		return static_cast<float>(dis(gen));
	}

	float randomFloat01()
	{
		return randomFloat(0, 1);
	}
};