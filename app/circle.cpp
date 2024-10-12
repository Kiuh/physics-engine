#pragma once

#include "glm/glm.hpp"
#include "color.cpp"
#include "math_tools.cpp"
#include "vertex_source.cpp"
#include "vec2_tools.cpp"
#include "shape.cpp"

struct Circle : public Shape
{
	static constexpr size_t circle_segment_splits = 6;
	static constexpr size_t circle_tris = 4 * (1 + circle_segment_splits);
	static constexpr float one_seg_deg = 360.0f / circle_tris;

	float radius;
	Color color;

	Circle()
	{
		radius = 1;
		color = Color::randomColor();
	}

	// From VertexSource
	std::vector<Vertex> getVertexes() const
	{
		std::vector<Vertex> vertices{};
		// around
		auto shift = glm::vec2(-radius, 0);
		for (size_t i = 0; i < getVertexesCount(); i++)
		{
			Vertex vert(tr->getPosition() + shift, color.getValue());
			vertices.push_back(vert);
			rotateVec2(shift, one_seg_deg);
		}
		return vertices;
	}

	size_t getVertexesCount() const
	{
		return static_cast<size_t>(circle_tris + 1);
	}

	std::vector<size_t> getIndexes() const
	{
		std::vector<size_t> result{};
		size_t ind = 1;
		for (size_t i = 0; i < circle_tris; i++)
		{
			result.push_back(0);
			result.push_back(ind);
			result.push_back(ind + 1);
			ind += 1;
		}
		result[result.size() - 1] = 1;
		return result;
	}

	size_t getIndexesCount() const
	{
		return static_cast<size_t>(3 * circle_tris);
	}

	// From Shape
	bool isOverlaps(Shape* shape)
	{
		return false;
	}

	Collision getCollision(Shape* shape)
	{
		return Collision{};
	}
};