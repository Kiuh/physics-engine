#include "graphic_engine.cpp"
#include <cstdlib>
#include <exception>
#include <iostream>
#include <ostream>

class App
{
	public:
	void run()
	{
		ge.run();
	}

	private:
	GraphicEngine ge{};
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