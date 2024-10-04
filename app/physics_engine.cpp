#pragma once

#include "aabb.cpp"
#include "data_provider.cpp"
#include "random_helper.cpp"
#include <boost/random.hpp>
#include <iostream>

class PhysicsEngine
{
	private:
	DataProvider* dataProvider;
	float timer;
	float interval;

	public:
	PhysicsEngine(DataProvider* dataProvider)
	{
		this->dataProvider = dataProvider;
		timer = 0.0f;
		interval = 0.5f;

		for (size_t i = 0; i < 5; i++)
		{
			addBox();
		}
	}
	float f = 0.0f;

	void addBox()
	{
		const glm::vec2 leftDown(-5, -5);
		const glm::vec2 rightUp(5, 5);
		const glm::vec2 center(randomFloat(leftDown.x, rightUp.x), randomFloat(leftDown.y, rightUp.y));
		const glm::vec2 shift(1.0f, 1.0f);

		AABB box{};
		box.setMinMax(center - shift, center + shift);

		box.setColor(getRandomColor());

		dataProvider->boxes.push_back(box);
	}

	glm::vec3 getRandomColor()
	{
		return glm::vec3{
			randomFloat01(),
			randomFloat01(),
			randomFloat01(),
		};
	}

	void update(float deltaTime)
	{
		//timer += deltaTime;

		if (timer >= interval)
		{
			addBox();
			timer = 0.0f;
		}
	}
};