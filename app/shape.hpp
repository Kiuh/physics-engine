#pragma once

#include "transform.hpp"
#include "vertex_source.hpp"

struct Shape : public VertexSource
{
	public:
	Transform& transform;

	Shape(Transform& tr) : transform(tr)
	{
	}
};