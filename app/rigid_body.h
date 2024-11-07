#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "transform.h"
#include "shape.h"
#include "collision.h"

struct RigidBody
{
	private:
	Transform& transform;

	public:
	glm::vec2 speed{};
	Shape& shape;

	bool isStatic = false;
	float mass = 1.0f;

	RigidBody(Transform& tr, Shape& shape);
	void update(float deltaTime);
	void addSpeed(glm::vec2& force);
	void moveToResolve(Collision col);

	private:
	void applyForces(float deltaTime);
};