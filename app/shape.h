#pragma once

#include "collision.hpp"
#include "transform.hpp"
#include "vertex_source.h"

class Shape : public VertexSource
{
	public:
	Transform& tr;

	Shape(Transform& tr);
};