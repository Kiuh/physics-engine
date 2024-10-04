#pragma once

#include "aabb.cpp"
#include "data_provider.cpp"
#include "random_helper.cpp"
#include "window_provider.cpp"
#include <boost/bind/bind.hpp>
#include <boost/random.hpp>
#include <iostream>
#include <vector>

static const float gravity_scale = 1.0f;

class PhysicsEngine
{
	private:
	DataProvider* data;
	size_t initialBoxCount = 5;

	public:
	PhysicsEngine(WindowProvider* win, DataProvider* data)
	{
		this->data = data;
		win->keyPressed.connect(boost::bind(&PhysicsEngine::processKeyPress, this, boost::placeholders::_1));
		createInitialObjects();
	}

	void createInitialObjects()
	{
		data->boxes.clear();
		for (size_t i = 0; i < initialBoxCount; i++)
		{
			createBox();
		}
		auto plate = AABB::make(glm::vec2{ -7, -6 }, glm::vec2{ 7, -5 }, Color{});
		data->addBox(plate);
	}

	void createBox()
	{
		const static glm::vec2 leftDown(-7, 0);
		const static glm::vec2 rightUp(7, 5);
		const static glm::vec2 shift(1.0f, 1.0f);

		glm::vec2 center(randomFloat(leftDown.x, rightUp.x), randomFloat(leftDown.y, rightUp.y));

		AABB box{};
		box.setMinMax(center - shift, center + shift);
		box.setColor(Color::randomColor());
		box.isStatic = false;
		data->addBox(box);
	}

	void processKeyPress(KeyCode key)
	{
		if (key == KeyCode::R)
		{
			createInitialObjects();
		}
	}

	void update(float deltaTime)
	{
		addGravity();
		resolveCollisions();
		applyVelocity(deltaTime);
	}

	void addGravity()
	{
		for (auto& box : data->boxes)
		{
			box.velocity = glm::vec2{};
			if (!box.isStatic)
			{
				box.velocity += glm::vec2{ 0,-1 } *gravity_scale;
			}
		}
	}

	void resolveCollisions()
	{
		for (size_t i = 0; i < data->boxes.size(); i++)
		{
			auto& box1 = data->boxes[i];
			if (box1.isStatic) continue;

			for (size_t j = 0; j < data->boxes.size(); j++)
			{
				if (i == j) continue;

				auto& box2 = data->boxes[j];

				if (AABB::isOverlaps(box1, box2))
				{
					box1.velocity += box1.center() - box2.center();
				}
			}
		}
	}

	void applyVelocity(float deltaTime)
	{
		for (auto& box : data->boxes)
		{
			if (!box.isStatic)
			{
				box.move(box.velocity * deltaTime);
			}
		}
	}
};