#pragma once

#pragma once

#include "app.cpp"

int main()
{
	App app{};

	try
	{
		app.run();
	}
	catch (const std::exception& e)
	{
		cerr << e.what() << endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}