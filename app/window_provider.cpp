#pragma once

#define GLFW_INCLUDE_VULKAN
#include "lodepng.h"
#include <GLFW/glfw3.h>
#include <boost/signals2.hpp>
#include <cstdint>
#include <filesystem>
#include <glm/ext/vector_int2.hpp>
#include <iostream>
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

		std::string filename = "icon.png";
		std::vector<unsigned char> buffer;
		std::vector<unsigned char> image;
		unsigned w, h;

		lodepng::load_file(buffer, filename);

		lodepng::State state;
		state.decoder.ignore_crc = 1;
		state.decoder.zlibsettings.ignore_adler32 = 1;

		unsigned error = lodepng::decode(image, w, h, state, buffer);

		if (error)
		{
			throw runtime_error("Decoder error : " + string(lodepng_error_text(error)));
		}

		GLFWimage images[1]{};

		images[0].width = w;
		images[0].height = h;
		images[0].pixels = image.data();

		glfwSetWindowIcon(window, 1, images);
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