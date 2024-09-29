#pragma once

#include "box.cpp"
#include "data_provider.cpp"

class PhysicsEngine
{
	private:
	DataProvider* dp;
	float timer;
	float shift;
	float interval;

	public:
	PhysicsEngine(DataProvider* dp)
	{
		this->dp = dp;
		timer = 0.0f;
		shift = 20.0f;
		interval = 4.0f;
	}

	void update(float deltaTime)
	{
		for (auto& box : dp->boxes)
		{
			for (auto& pos : box.pos)
			{
				pos += (shift / interval) * deltaTime;
			}
		}
		timer += deltaTime;

		if (timer >= interval)
		{
			shift *= -1.0f;
			timer = 0.0f;
		}
	}
};