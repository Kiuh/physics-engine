#pragma once

#include "aabb.cpp"
#include "data_manager.cpp"
#include "random_helper.cpp"
#include "window_manager.cpp"

#include <boost/bind/bind.hpp>
#include <boost/random.hpp>
#include <iostream>
#include <vector>

static const float gravity_scale = 3.0f;

class PhysicsEngine
{
	private:
	DataManager* data;
	size_t initialBoxCount = 10;

	public:
	PhysicsEngine(WindowManager* win, DataManager* data)
	{
		this->data = data;
		win->keyPressed.connect(boost::bind(&PhysicsEngine::processKeyPress, this, boost::placeholders::_1));
		createInitialObjects();
	}

	void createInitialObjects()
	{
		data->data_mutex.lock();
		data->clearObjects();
		for (size_t i = 0; i < initialBoxCount; i++)
		{
			createBox();
		}
		auto plate = AABB::make(glm::vec2{ -10, -8 }, glm::vec2{ 10, -5 }, Color{});
		plate->mass = 100.0f;
		data->addAABB(plate);
		data->data_mutex.unlock();
		data->notifyStructureChanging();
	}

	void createBox()
	{
		const static glm::vec2 leftDown(-7, 0);
		const static glm::vec2 rightUp(7, 5);
		const static glm::vec2 shift(1.0f, 1.0f);
		const static float mass = 3.0f;

		glm::vec2 center(randomFloat(leftDown.x, rightUp.x), randomFloat(leftDown.y, rightUp.y));

		AABB* box = AABB::make(center - shift, center + shift, Color::randomColor());
		box->isStatic = false;
		box->mass = mass;
		data->addAABB(box);
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
		calculateVelocity(deltaTime);
		applyVelocity(deltaTime);
	}

	void calculateVelocity(float deltaTime)
	{
		for (auto box : data->boxes)
		{
			// Refresh
			box->velocity = glm::vec2{};

			// Add gravity
			box->velocity += glm::vec2{ 0,-1 } *gravity_scale * box->mass;
		}

		// Simple resolve collision
		for (size_t i = 0; i < data->boxes.size(); i++)
		{
			auto& box1 = *data->boxes[i];
			for (size_t j = i + 1; j < data->boxes.size(); j++)
			{
				auto& box2 = *data->boxes[j];
				if (AABB::isOverlaps(box1, box2))
				{
					auto force = AABB::getShortestOverlap(box1, box2);
					if (!box1.isStatic)
						box1.move(force * box2.mass * deltaTime);
					if (!box2.isStatic)
						box2.move(-(force * box1.mass * deltaTime));
				}
			}
		}
	}

	void applyVelocity(float deltaTime)
	{
		for (auto box : data->boxes)
		{
			if (!box->isStatic)
			{
				box->move(box->velocity * deltaTime);
			}
		}
	}
};