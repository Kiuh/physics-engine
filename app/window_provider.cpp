#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <boost/signals2.hpp>
#include <cstdint>
#include <glm/ext/vector_int2.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

using namespace std;
using namespace glm;
using namespace boost::signals2;

class WindowProvider
{
	private:
	ivec2 size;
	string title;
	GLFWwindow* window;

	public:
	signal<void()> windowResized;

	WindowProvider(ivec2 size, string title)
	{
		this->size = size;
		this->title = title;

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	ivec2 getSize() const
	{
		return size;
	}

	void poolEvents()
	{
		glfwPollEvents();
	}

	void waitForNoZero()
	{
		glfwGetFramebufferSize(window, &size.x, &size.y);
		while (size.x == 0 || size.y == 0)
		{
			glfwGetFramebufferSize(window, &size.x, &size.y);
		}
	}

	bool isShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto window_provider = reinterpret_cast<WindowProvider*>(glfwGetWindowUserPointer(window));
		window_provider->windowResized();
	}

	vector<const char*> getVulkanExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}

	VkSurfaceKHR createVkSurface(VkInstance* instance)
	{
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(*instance, window, nullptr, &surface) != VK_SUCCESS)
		{
			throw runtime_error("failed to create window surface!");
		}
		return surface;
	}

	~WindowProvider()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};