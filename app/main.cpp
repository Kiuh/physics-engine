#pragma once

#include "draw_data_provider.cpp"
#include "graphic_engine.cpp"
#include "window_provider.cpp"
#include <cstdlib>
#include <exception>
#include <glm/ext/vector_uint2.hpp>
#include <iostream>
#include <ostream>
#include <string>

class App
{
	private:
	glm::uvec2 size{ 1000, 1000 };
	std::string title = "Physics Simulation";

	WindowProvider* wp;
	GraphicEngine* ge;
	DrawDataProvider* dp;

	public:
	App()
	{
		this->dp = new DrawDataProvider();
		this->wp = new WindowProvider(size, title);
		this->ge = new GraphicEngine(wp, dp);
	}

	void run()
	{
		while (!wp->isShouldClose())
		{
			wp->poolEvents();
			ge->draw();
		}
	}

	~App()
	{
		delete this->dp;
		delete this->ge;
		delete this->wp;
	}
};

int main()
{
	App app{};

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}