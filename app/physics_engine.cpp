#pragma once

#include "box.cpp"
#include "data_provider.cpp"
#include <boost/random.hpp>
#include <boost/random/uniform_01.hpp>
#include <boost/random/uniform_real.hpp>

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

		//box.color = getRandomColor();
		box.color = { 0,1,0 };

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
		minstd_rand rng{};
		uniform_real<float> unit(a, b);
		variate_generator<minstd_rand&, uniform_real<float>> generator(rng, unit);
		return generator();
	}

	float random01Float()
	{
		minstd_rand rng{};
		uniform_01 unit{};
		variate_generator<minstd_rand&, uniform_01<>> generator(rng, unit);
		return generator();
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