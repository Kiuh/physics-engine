#include "object.h"

Object::Object(Shape& shapeRef)
{
	tr = &shapeRef.tr;
	shape = &shapeRef;
	rigidBody = new RigidBody(*tr, *shape);
}