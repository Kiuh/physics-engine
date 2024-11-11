#include "object.h"

Object::Object(Shape& shapeRef)
{
	transform = &shapeRef.tr;
	shape = &shapeRef;
	rigidBody = new RigidBody(*transform, *shape);
}