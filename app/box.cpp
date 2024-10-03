#pragma once

#include "vertex.cpp"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <vector>

static constexpr const size_t BOX_VERTEX_COUNT = 6;
static constexpr const size_t BOX_INDECES[BOX_VERTEX_COUNT] = { 0,1,2,2,3,0 };

struct Box
{
	glm::vec2 pos[4];
	glm::vec3 color;
	std::vector<Vertex> vertices{};

	void calculateVertices()
	{
		vertices.resize(BOX_VERTEX_COUNT);
		for (size_t i = 0; i < BOX_VERTEX_COUNT; i++)
		{
			vertices[i].pos = pos[BOX_INDECES[i]];
			vertices[i].color = color;
		}
	}
};