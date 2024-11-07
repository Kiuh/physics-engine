#pragma once

#include <vector>
#include "vertex.h"

class VertexSource
{
	public:
	virtual std::vector<Vertex> getVertexes() const = 0;
	virtual std::vector<size_t> getIndexes() const = 0;
	virtual size_t getVertexesCount() const = 0;
	virtual size_t getIndexesCount() const = 0;
};