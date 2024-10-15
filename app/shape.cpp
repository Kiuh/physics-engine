#pragma once

#include "transform.cpp"
#include "vertex_source.cpp"

struct Shape : public VertexSource
{
	public:
	Transform& transform;

	Shape(Transform& tr) : transform(tr)
	{
	}
};