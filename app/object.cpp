#pragma once

#include "rigid_body.cpp"
#include "shape.cpp"
#include <memory>
#include <type_traits>

class Object
{
	public:
	Transform* transform;
	Shape* shape;
	RigidBody* rigidBody;

	Object()
	{
		transform = new Transform();
		shape = nullptr;
		rigidBody = new RigidBody(transform);
	}

	void setShape(Shape* shape)
	{
		this->shape = shape;
		this->shape->setTransform(transform);
		rigidBody->setShape(shape);
	}
};