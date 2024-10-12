#pragma once

#include "data_manager.cpp"
#include "random_helper.cpp"
#include "window_manager.cpp"
#include "object.cpp"
#include "rigid_body.cpp"
#include "collision_resolver_builder.cpp"

#include <boost/bind/bind.hpp>
#include <boost/random.hpp>
#include <iostream>
#include <vector>

static const float gravity_scale = 5.0f;

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
			auto shift = glm::vec2{ 0,-1 } *gravity_scale * rb->mass;
			rb->addForce(shift);
		}

		// Collision
		for (size_t i = 0; i < rigidBodies.size(); i++)
		{
			for (size_t j = i + 1; j < rigidBodies.size(); j++)
			{
				resolveCollision(rigidBodies[i], rigidBodies[j]);
			}
		}

		// Apply
		for (auto rb : rigidBodies)
		{
			rb->update(deltaTime);
		}

		process_mutex.unlock();
	}

	void resolveCollision(RigidBody* rb1, RigidBody* rb2)
	{
		auto resolver = createCollisionResolver(rb1->shape, rb2->shape);

		if (!resolver->isOverlaps())
		{
			return;
		}

		applyCollisionForce(resolver->getForwardCollision(), rb1, rb2);
		applyCollisionForce(resolver->getReverseCollision(), rb2, rb1);

		delete resolver;
	}

	void applyCollisionForce(Collision col, RigidBody* rb1, RigidBody* rb2)
	{
		auto force = col.depth * col.normal * rb2->mass;
		rb1->addForce(force);
	}
};