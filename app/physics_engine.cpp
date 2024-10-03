#pragma once

#include "box.cpp"
#include "data_provider.cpp"
#include <boost/random.hpp>
#include <iostream>

using namespace boost;
using namespace boost::random;

class PhysicsEngine
{
	private:
	DataProvider* dp;
	float timer;
	float interval;

	public:
	PhysicsEngine(DataProvider* dp)
	{
		this->dp = dp;
		timer = 0.0f;
		interval = 0.5f;

		addBox();
	}

	void addBox()
	{
		const vec2 leftDown(-5, -5);
		const vec2 rightUp(5, 5);
		const vec2 center(randomFloat(leftDown.x, rightUp.x), randomFloat(leftDown.y, rightUp.y));
		const vec2 shift(1.0f, 1.0f);

		Box box{};

		box.pos[0] = vec2(center.x - shift.x, center.y + shift.y);
		box.pos[1] = center + shift;
		box.pos[2] = vec2(center.x + shift.x, center.y - shift.y);
		box.pos[3] = center - shift;

		box.color = getRandomColor();

		dp->boxes.push_back(box);
	}

	vec3 getRandomColor()
	{
		return vec3{
			random01Float(),
			random01Float(),
			random01Float(),
		};
	}

	float randomFloat(float a, float b)
	{
		mt19937 rng;
		uniform_real_distribution<> gen(a, b);
		auto result = gen(rng);
		return static_cast<float>(result);
	}

	float random01Float()
	{
		return randomFloat(0, 1);
	}

	void update(float deltaTime)
	{
		timer += deltaTime;

		if (timer >= interval)
		{
			addBox();
			timer = 0.0f;
		}
	}
};