#pragma once

#include "vertex.cpp"
#include "window_provider.cpp"
#include <cstdint>

class VertexTransformer
{
	private:
	WindowProvider* wp;

	uint32_t pixelsPerUnit = 50;

	public:
	VertexTransformer(WindowProvider* wp)
	{
		this->wp = wp;
	}

	void worldToScreen(Vertex* vert)
	{
		auto screen = wp->getSize();
		screen /= 2.0f;
		vert->pos *= pixelsPerUnit;
		vert->pos /= screen;

		vert->pos.y -= 1;
		vert->pos.y *= -1;
	}
};