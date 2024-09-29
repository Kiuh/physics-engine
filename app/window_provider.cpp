#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/ext/vector_uint2.hpp>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

class WindowProvider
{
	private:
	glm::uvec2 size;
	std::string title;
	GLFWwindow* window;
	bool windowResized;

	public:
	WindowProvider(glm::uvec2 size, std::string title)
	{
		this->size = size;
		this->title = title;
		windowResized = false;

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	}

	glm::uvec2 getSize() const
	{
		return size;
	}

	bool isWindowResized() const
	{
		return windowResized;
	}

	void setWindowResizedDone()
	{
		windowResized = false;
	}

	void poolEvents()
	{
		glfwPollEvents();
	}

	void waitForNoZero()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0)
		{
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}
	}

	bool isShouldClose()
	{
		return glfwWindowShouldClose(window);
	}

	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto window_provider = reinterpret_cast<WindowProvider*>(glfwGetWindowUserPointer(window));
		window_provider->windowResized = true;
	}

	std::vector<const char*> getVulkanExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}

	VkSurfaceKHR createVkSurface(VkInstance* instance)
	{
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(*instance, window, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
		return surface;
	}

	~WindowProvider()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}
};