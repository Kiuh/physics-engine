#include "physics_engine.h"

void PhysicsEngine::update(float deltaTime)
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
			resolveImpulseCollision(*rigidBodies[i], *rigidBodies[j]);
		}
	}

	// Apply
	for (auto rb : rigidBodies)
	{
		rb->update(deltaTime);
	}

	process_mutex.unlock();
}

void PhysicsEngine::resolveImpulseCollision(RigidBody& rb1, RigidBody& rb2) const
{
	if (!detectCollision(rb1.shape, rb2.shape))
	{
		return;
	}

	auto fc = resolveCollision(rb1.shape, rb2.shape);
	auto rc = resolveCollision(rb2.shape, rb1.shape);

	applyCollisionForce(fc, rb1, rb2);
	applyCollisionForce(rc, rb2, rb1);
}

void PhysicsEngine::applyCollisionForce(Collision col, RigidBody& rb1, RigidBody& rb2) const
{
	rb1.speed *= glm::abs(mt::rotate90(col.normal));
	rb1.moveToResolve(col);
}