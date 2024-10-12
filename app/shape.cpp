#pragma once

#include "transform.cpp"
#include "vertex_source.cpp"

struct Shape : public VertexSource
{
	public:
	Transform* transform = nullptr;

	void setTransform(Transform* tr)
	{
		this->transform = tr;
	}
};