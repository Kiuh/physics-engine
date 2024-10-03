#pragma once

#include "box.cpp"
#include "data_provider.cpp"
#include "random_helper.cpp"
#include <boost/random.hpp>
#include <iostream>

class PhysicsEngine
{
	private:
	DataProvider* dataProvider;
	std::unique_ptr<RandomHelper> randomHelper;
	float timer;
	float interval;

	public:
	PhysicsEngine(DataProvider* dp)
	{
		this->dataProvider = dp;
		timer = 0.0f;
		interval = 0.5f;

		randomHelper = std::make_unique<RandomHelper>();

		addBox();
	}
	float f = 0.0f;

	void addBox()
	{
		const glm::vec2 leftDown(-5, -5);
		const glm::vec2 rightUp(5, 5);
		const glm::vec2 center(randomHelper->randomFloat(leftDown.x, rightUp.x), randomHelper->randomFloat(leftDown.y, rightUp.y));
		const glm::vec2 shift(1.0f, 1.0f);

		Box box{};

		box.pos[0] = glm::vec2(center.x - shift.x, center.y + shift.y);
		box.pos[1] = center + shift;
		box.pos[2] = glm::vec2(center.x + shift.x, center.y - shift.y);
		box.pos[3] = center - shift;

		box.color = getRandomColor();

		dataProvider->boxes.push_back(box);
	}

	glm::vec3 getRandomColor()
	{
		return glm::vec3{
			randomHelper->randomFloat01(),
			randomHelper->randomFloat01(),
			randomHelper->randomFloat01(),
		};
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