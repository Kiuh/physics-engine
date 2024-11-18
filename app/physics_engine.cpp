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
			auto& rb1 = *rigidBodies[i];
			auto& rb2 = *rigidBodies[j];

			if (isOverlaps(rb1.shape, rb2.shape))
			{
				to_color.insert(&rb1.shape);
				to_color.insert(&rb2.shape);

				auto res1 = intersection(rb1.shape, rb2.shape);
				auto res2 = intersection(rb2.shape, rb1.shape);

				if (res1.has_value())
				{
					auto& col = res1.value();
					rb1.moveToResolve(-col.normal * col.penetration / 2.0f);
				}

				if (res2.has_value())
				{
					auto& col = res2.value();
					rb2.moveToResolve(-col.normal * col.penetration / 2.0f);
				}
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