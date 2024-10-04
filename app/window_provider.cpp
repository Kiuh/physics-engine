#pragma once

#define GLFW_INCLUDE_VULKAN
#include "lodepng.h"
#include <GLFW/glfw3.h>
#include <boost/signals2.hpp>
#include <cstdint>
#include <filesystem>
#include <glm.hpp>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

class WindowProvider
{
	private:
	glm::ivec2 size;
	std::string title;
	GLFWwindow* window;

	public:
	boost::signals2::signal<void()> windowResized;

	WindowProvider(glm::ivec2 size, std::string title)
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
			throw std::runtime_error("Decoder error : " + std::string(lodepng_error_text(error)));
		}

		GLFWimage images[1]{};

		images[0].width = w;
		images[0].height = h;
		images[0].pixels = image.data();

		glfwSetWindowIcon(window, 1, images);
	}

	glm::ivec2 getSize() const
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
		glfwGetFramebufferSize(window, &window_provider->size.x, &window_provider->size.y);
		window_provider->windowResized();
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