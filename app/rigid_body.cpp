#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "transform.cpp"

struct RigidBody
{
	Transform* transform = nullptr;

	float inverseMass = 0.1f;
	glm::vec2 velocity{};

	glm::vec2 forceAccumulator{};
	glm::vec2 torqueAccumulator{};

	RigidBody(Transform* tr)
	{
		this->transform = tr;
	}

	void update(float deltaTime)
	{
		applyForces(deltaTime);
		clearAccumulators();
	}

	void applyForces(float deltaTime)
	{
		transform->movePosition(forceAccumulator * deltaTime);
	}

	void addForce(glm::vec2& force)
	{
		forceAccumulator += force;
	}

	void clearAccumulators()
	{
		forceAccumulator = {};
		torqueAccumulator = {};
	}
};