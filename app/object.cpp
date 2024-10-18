#pragma once

#include "rigid_body.cpp"
#include "shape.hpp"
#include <memory>
#include <type_traits>

class Object
{
	public:
	Transform* transform;
	Shape* shape;
	RigidBody* rigidBody;

	Object(Shape* shapeRef)
	{
		transform = &shapeRef->transform;
		shape = shapeRef;
		rigidBody = new RigidBody(transform, shape);
	}
};