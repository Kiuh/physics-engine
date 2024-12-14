#pragma once

#include "rigid_body.h"
#include "shape.h"
#include <memory>
#include <type_traits>

class Object
{
	public:
	Transform* tr;
	Shape* shape;
	RigidBody* rigidBody;

	Object(Shape& shapeRef);
};