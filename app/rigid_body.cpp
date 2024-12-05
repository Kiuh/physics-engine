#include "rigid_body.h"

RigidBody::RigidBody(Transform& tr, Shape& shape) : transform(tr), shape(shape)
{
}

void RigidBody::update(float deltaTime)
{
	if (isStatic)
	{
		force = { 0.0f, 0.0f };
		return;
	}

	acceleration = force / mass;
	linearVelocity += acceleration * deltaTime;
	transform.movePos(linearVelocity * deltaTime);
	transform.rotate(rotationVelocity * deltaTime);

	force = { 0.0f, 0.0f };
}

void RigidBody::addForce(glm::vec2 amount)
{
	force += amount;
}

void RigidBody::move(glm::vec2 amount)
{
	transform.movePos(amount);
}

float RigidBody::invMass() const
{
	return isStatic ? 0.0f : 1.0f / mass;
}
