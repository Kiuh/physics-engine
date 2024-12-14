#pragma once

#include "collision.h"
#include "glm/glm.hpp"
#include "imgui.h"
#include "shape.h"
#include "transform.h"

struct RigidBody
{
	public:
	Transform& tr;
	Shape& shape;

	glm::vec2 linearVelocity{};
	glm::vec2 acceleration{};
	float angularVelocity{};

	glm::vec2 force{};

	bool isStatic;
	float mass;
	float inertia;
	float area;
	float restitution;

	RigidBody(Transform& tr, Shape& shape);
	void update(float deltaTime);
	void addForce(glm::vec2 amount);
	void move(glm::vec2 amount);

	float density() const;
	float invMass() const;
	float invInertia() const;

	void drawDebugTooltip() const;

	private:
	float calcInertia() const;
};