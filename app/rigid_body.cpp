#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/quaternion.hpp"
#include "transform.hpp"
#include "shape.hpp"
#include "collision.hpp"

struct RigidBody
{
	private:
	Transform* transform;

	public:
	glm::vec2 speed{};
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
		applyForces(deltaTime);
	}

	void addSpeed(glm::vec2& force)
	{
		speed += force;
	}

	void moveToResolve(Contact cont)
	{
		if (isStatic) return;
		transform->movePos(cont.normal * cont.penetration);
	}

	private:
	void applyForces(float deltaTime)
	{
		if (isStatic) return;
		transform->movePos(speed * deltaTime);
	}
};