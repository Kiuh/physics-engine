#pragma once

#include "collision.hpp"
#include "transform.hpp"
#include "vertex_source.h"

class Shape : public VertexSource
{
	public:
	Transform& tr;

	Shape(Transform& tr) : tr(tr)
	{
	}

	virtual Collision getCollision(Shape& other) const = 0;
	virtual bool isCollide(Shape& other) const = 0;
};