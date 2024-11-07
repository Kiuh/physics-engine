#pragma once

#include "rigid_body.h"
#include "shape.h"
#include <memory>
#include <type_traits>

class Object
{
	public:
	Transform* transform;
	Shape* shape;
	RigidBody* rigidBody;

	Object(Shape* shapeRef);
};