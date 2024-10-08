#pragma once

#include<vector>
#include"vertex.cpp"

struct VertexSource
{
	virtual std::vector<Vertex> getVertexes() const
	{
		return std::vector<Vertex>();
	}

	virtual std::vector<size_t> getIndexes() const
	{
		return std::vector<size_t>();
	}

	virtual size_t getVertexesCount() const
	{
		return 0;
	}

	virtual size_t getIndexesCount() const
	{
		return 0;
	}
};