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

	Object(Shape* shapeRef)
	{
		transform = new Transform();
		shape = shapeRef;
		shape->setTransform(transform);
		rigidBody = new RigidBody(transform, shape);
	}
};