#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "transform.cpp"
#include "shape.cpp"

struct RigidBody
{
	private:
	Transform* transform;

	glm::vec2 velocity{};

	glm::vec2 forceAccumulator{};
	glm::vec2 torqueAccumulator{};

	public:
	Shape* shape;

	bool isStatic = false;
	float mass = 1.0f;

	RigidBody(Transform* tr, Shape* shape)
	{
		this->transform = tr;
		this->shape = shape;
	}

	void update(float deltaTime)
	{
		if (isStatic)
		{
			return;
		}

		applyForces(deltaTime);
		clearAccumulators();
	}

	void addForce(glm::vec2& force)
	{
		forceAccumulator += force;
	}

	private:
	void applyForces(float deltaTime)
	{
		transform->movePosition(forceAccumulator * deltaTime);
	}

	void clearAccumulators()
	{
		forceAccumulator = { 0,0 };
		torqueAccumulator = { 0,0 };
	}
};