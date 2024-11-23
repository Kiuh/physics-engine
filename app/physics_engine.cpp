#include "physics_engine.h"

PhysicsEngine::PhysicsEngine(Debug* debug)
{
	debug->buildUI.connect(boost::bind(&PhysicsEngine::buildDebugUI, this));
}

void PhysicsEngine::update(float deltaTime)
{
	process_mutex.lock();

	// Gravity
	for (auto rb : rigidBodies)
	{
		rb->addForce(gravity);
	}

	for (auto rb : rigidBodies)
	{
		rb->update(deltaTime);
	}

	std::set<Shape*> to_color{};

	// Collision
	for (size_t i = 0; i < rigidBodies.size(); i++)
	{
		for (size_t j = i + 1; j < rigidBodies.size(); j++)
		{
			auto& rb1 = *rigidBodies[i];
			auto& rb2 = *rigidBodies[j];

			if (rb1.isStatic && rb2.isStatic) continue;

			if (isOverlaps(rb1.shape, rb2.shape))
			{
				to_color.insert(&rb1.shape);
				to_color.insert(&rb2.shape);

				auto res1 = intersection(rb1.shape, rb2.shape);

				if (res1.has_value())
				{
					auto& col = res1.value();
					if (rb1.isStatic)
					{
						rb2.move(col.normal * col.penetration);
					}
					else if (rb2.isStatic)
					{
						rb1.move(-col.normal * col.penetration);
					}
					else
					{
						rb1.move(-col.normal * col.penetration / 2.0f);
						rb2.move(col.normal * col.penetration / 2.0f);
					}

					resolveCollision(rb1, rb2, col);
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

	process_mutex.unlock();
}

void PhysicsEngine::buildDebugUI()
{
	ImGui::Begin("Physics parameters", nullptr, 0);
	ImGui::DragFloat2("Gravity", &gravity.x, 0.1f, -50.0f, 50.0f, "%.2f");
	ImGui::End();
}

void PhysicsEngine::resolveCollision(RigidBody& rb1, RigidBody& rb2, Collision col)
{
	auto relativeVelocity = rb2.linearVelocity - rb1.linearVelocity;

	if (glm::dot(relativeVelocity, col.normal) > 0.001f)
	{
		return;
	}

	float e = std::fmin(rb1.restitution, rb2.restitution);

	float j = -(1.0f + e) * glm::dot(relativeVelocity, col.normal);
	j /= rb1.invMass() + rb2.invMass();

	auto impulse = j * col.normal;

	rb1.linearVelocity -= impulse * rb1.invMass();
	rb2.linearVelocity += impulse * rb2.invMass();
}
