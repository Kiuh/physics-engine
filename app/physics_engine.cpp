#pragma once

#include "aabb.cpp"
#include "data_manager.cpp"
#include "random_helper.cpp"
#include "window_manager.cpp"
#include "object.cpp"
#include "rigid_body.cpp"

#include <boost/bind/bind.hpp>
#include <boost/random.hpp>
#include <iostream>
#include <vector>

static const float gravity_scale = 3.0f;

class PhysicsEngine
{
	public:
	std::vector<RigidBody*> rigidBodies;
	std::mutex process_mutex;

	void update(float deltaTime)
	{
		process_mutex.lock();

		// Gravity
		for (auto rb : rigidBodies)
		{
			auto shift = glm::vec2{ 0,-1 } *gravity_scale * rb->inverseMass;
			rb->addForce(shift);
		}

		// Apply
		for (auto rb : rigidBodies)
		{
			rb->applyForces(deltaTime);
		}

		process_mutex.unlock();
	}
};