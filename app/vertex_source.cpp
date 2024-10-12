#pragma once

#include<vector>
#include <stdexcept>
#include"vertex.cpp"

struct VertexSource
{
	virtual std::vector<Vertex> getVertexes() const
	{
		throw std::runtime_error("Called getVertexes from raw VertexSource!");
	}

	virtual std::vector<size_t> getIndexes() const
	{
		throw std::runtime_error("Called getIndexes from raw VertexSource!");
	}

	virtual size_t getVertexesCount() const
	{
		throw std::runtime_error("Called getVertexesCount from raw VertexSource!");
	}

	virtual size_t getIndexesCount() const
	{
		throw std::runtime_error("Called getIndexesCount from raw VertexSource!");
	}
};