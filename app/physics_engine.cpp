#include "physics_engine.h"

static const glm::vec2 gravity = { 0, -9.8f };

void PhysicsEngine::update(float deltaTime)
{
	process_mutex.lock();

	// Gravity
	for (auto rb : rigidBodies)
	{
		auto frameNewSpeed = gravity * deltaTime;
		rb->addSpeed(frameNewSpeed);
	}

	std::set<Shape*> to_color{};

	// Collision
	for (size_t i = 0; i < rigidBodies.size(); i++)
	{
		for (size_t j = i + 1; j < rigidBodies.size(); j++)
		{
			if (detectCollision(rigidBodies[i]->shape, rigidBodies[j]->shape))
			{
				to_color.insert(&rigidBodies[i]->shape);
			}

			if (detectCollision(rigidBodies[j]->shape, rigidBodies[i]->shape))
			{
				to_color.insert(&rigidBodies[j]->shape);
			}
		}
	}

	for (auto rb : rigidBodies)
	{
		rb->shape.color = Color::white();
	}

	for (auto sh : to_color)
	{
		sh->color = Color::red();
	}

	// Apply
	for (auto rb : rigidBodies)
	{
		rb->update(deltaTime);
	}

	process_mutex.unlock();
}