#pragma once

#include "collision.h"
#include "transform.h"
#include "vertex_source.h"

class Shape : public VertexSource
{
	public:
	Transform& tr;

	Shape(Transform& tr);
};