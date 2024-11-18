#pragma once

#include "collision.h"
#include "glm/glm.hpp"
#include "shape.h"
#include "transform.h"

struct RigidBody
{
	private:
	Transform& transform;

	public:
	Shape& shape;

	glm::vec2 linearVelocity{};
	glm::vec2 acceleration{};
	float rotationVelocity = 0;

	glm::vec2 force{};

	bool isStatic = false;
	float mass = 1.0f;
	float restitution = 0.6f;
	float density = 2.0f;

	RigidBody(Transform& tr, Shape& shape);
	void update(float deltaTime);
	void addForce(glm::vec2 amount);
	void move(glm::vec2 amount);

	float invMass() const;
};