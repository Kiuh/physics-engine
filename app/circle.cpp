#pragma once

#include <glm.hpp>
#include "color.cpp"
#include "math_tools.cpp"
#include "vertex_source.cpp"

constexpr size_t circle_segment_splits = 6;
constexpr size_t circle_tris = 4 * (1 + circle_segment_splits);
constexpr float one_seg_deg = 360.0f / circle_tris;

struct Circle : VertexSource
{
	// View properties
	glm::vec2 center{ 0,0 };
	float radius = 1;
	Color color{ glm::vec3{ 1.0f, 1.0f, 1.0f } };

	static Circle* make(glm::vec2 center, float radius, Color color)
	{
		Circle* circle = new Circle{};
		circle->center = center;
		circle->radius = radius;
		circle->color = color;
		return circle;
	}

	std::vector<Vertex> getVertexes() const
	{
		std::vector<Vertex> vertices{};
		// around
		auto shift = glm::vec2(-radius, 0);
		for (size_t i = 0; i < circle_tris; i++)
		{
			auto vert = Vertex::make(center + shift, color.getValue());
			vertices.push_back(vert);
			rotateForOneSeg(shift, one_seg_deg);
		}
		return vertices;
	}

	void rotateForOneSeg(glm::vec2& vec, float angle) const
	{
		vec = rotateVec2(vec, angle);
	}

	glm::vec2 rotateVec2(glm::vec2& v, float angle) const
	{
		float rad = glm::radians(angle);
		glm::mat2 rotationMatrix = glm::mat2(
			glm::cos(rad), -glm::sin(rad),
			glm::sin(rad), glm::cos(rad)
		);
		return rotationMatrix * v;
	}

	size_t getVertexesCount() const
	{
		return static_cast<size_t>(1 + circle_tris);
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
};