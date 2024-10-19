#pragma once

#include "rigid_body.cpp"
#include "shape.h"
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
		transform = &shapeRef->tr;
		shape = shapeRef;
		rigidBody = new RigidBody(*transform, *shape);
	}
};