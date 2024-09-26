#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <exception>
#include <iostream>

class App
{
	public:
	void run()
	{
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

	private:
	int width = 1280;
	int height = 720;
	GLFWwindow* window = nullptr;

	void initWindow()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		window = glfwCreateWindow(width, height, "Vulkan", nullptr, nullptr);
	}

	void initVulkan()
	{

	}

	void mainLoop()
	{
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void cleanup()
	{
		glfwDestroyWindow(window);

		glfwTerminate();
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