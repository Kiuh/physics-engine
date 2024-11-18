#pragma once

#include "vertex.h"
#include <vector>

struct VertexSource
{
	virtual std::vector<Vertex> getVertexes() const = 0;
	virtual std::vector<size_t> getIndexes() const = 0;
	virtual size_t getVertexesCount() const = 0;
	virtual size_t getIndexesCount() const = 0;
};