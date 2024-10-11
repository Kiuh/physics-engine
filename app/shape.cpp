#pragma once

#include "glm/glm.hpp"
#include "collision.cpp"
#include "transform.cpp"
#include "vertex_source.cpp"

struct Shape : public VertexSource
{
	protected:
	Transform* tr = nullptr;

	public:
	void setTransform(Transform* tr)
	{
		this->tr = tr;
	}

	virtual bool isOverlaps(Shape* shape)
	{
		return false;
	}

	virtual Collision getCollision(Shape* shape)
	{
		return Collision{};
	}
};