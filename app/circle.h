#pragma once

#include "glm/glm.hpp"
#include "color.h"
#include "math_tools.h"
#include "vertex_source.h"
#include "vec2_tools.h"
#include "shape.h"

class Circle : public Shape
{
	static constexpr size_t circle_segment_splits = 6;
	static constexpr size_t circle_tris = 4 * (1 + circle_segment_splits);
	static constexpr float one_seg_deg = 360.0f / circle_tris;

	public:
	float radius;
	Color color;

	Circle(Transform& tr);

	std::vector<Vertex> getVertexes() const;
	size_t getVertexesCount() const;
	std::vector<size_t> getIndexes() const;
	size_t getIndexesCount() const;
};