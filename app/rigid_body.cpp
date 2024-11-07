#include "rigid_body.h"

RigidBody::RigidBody(Transform& tr, Shape& shape) : transform(tr), shape(shape)
{
}

void RigidBody::update(float deltaTime)
{
	applyForces(deltaTime);
}

void RigidBody::addSpeed(glm::vec2& force)
{
	speed += force;
}

void RigidBody::moveToResolve(Collision col)
{
	if (isStatic) return;
	transform.movePos(col.normal * col.penetration);
}

void RigidBody::applyForces(float deltaTime)
{
	if (isStatic) return;
	transform.movePos(speed * deltaTime);
}
