#pragma once

#include "data_manager.cpp"
#include "random_helper.hpp"
#include "window_manager.cpp"
#include "object.cpp"
#include "rigid_body.cpp"
#include "detectors.h"
#include "resolvers.h"

#include <boost/bind/bind.hpp>
#include <iostream>
#include <vector>

static const glm::vec2 gravity = { 0, -9.8f };

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
			auto frameNewSpeed = gravity * deltaTime;
			rb->addSpeed(frameNewSpeed);
		}

		// Collision
		for (size_t i = 0; i < rigidBodies.size(); i++)
		{
			for (size_t j = i + 1; j < rigidBodies.size(); j++)
			{
				resolveCollision(*rigidBodies[i], *rigidBodies[j]);
			}
		}

		// Apply
		for (auto rb : rigidBodies)
		{
			rb->update(deltaTime);
		}

		process_mutex.unlock();
	}

	void resolveCollision(RigidBody& rb1, RigidBody& rb2) const
	{
		if (!detectCollision(rb1.shape, rb2.shape))
		{
			return;
		}

		auto fc = createCollision(rb1.shape, rb2.shape);
		auto rc = createCollision(rb2.shape, rb1.shape);

		applyCollisionForce(fc, rb1, rb2);
		applyCollisionForce(rc, rb2, rb1);
	}

	void applyCollisionForce(Collision col, RigidBody& rb1, RigidBody& rb2) const
	{
		rb1.speed *= glm::abs(mt::rotate90(col.contact.normal));
		rb1.moveToResolve(col.contact);
	}
};