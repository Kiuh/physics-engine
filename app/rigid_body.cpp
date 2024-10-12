#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "transform.cpp"
#include "shape.cpp"

struct RigidBody
{
	Transform* transform;
	Shape* shape;

	float inverseMass = 0.1f;
	glm::vec2 velocity{};

	glm::vec2 forceAccumulator{};
	glm::vec2 torqueAccumulator{};

	RigidBody(Transform* tr)
	{
		this->transform = tr;
	}

	void setShape(Shape* shape)
	{
		this->shape = shape;
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
		forceAccumulator = { 0,0 };
		torqueAccumulator = { 0,0 };
	}
};