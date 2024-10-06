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

enum KeyCode
{
	Undefined,
	R,
};

class WindowManager
{
	private:
	glm::ivec2 size{};
	std::string title{};
	GLFWwindow* window = nullptr;

	public:
	boost::signals2::signal<void()> windowResized{};
	boost::signals2::signal<void(KeyCode)> keyPressed{};

	WindowManager(glm::ivec2 size, std::string title)
	{
		this->size = size;
		this->title = title;

		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
		glfwSetKeyCallback(window, keyCallback);
		setIcon();
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

	std::vector<const char*> getVulkanExtensions()
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
	}

	VkSurfaceKHR createVkSurface(VkInstance& instance)
	{
		VkSurfaceKHR surface;
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
		return surface;
	}

	~WindowManager()
	{
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	private:
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
	{
		auto window_provider = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
		glfwGetFramebufferSize(window, &window_provider->size.x, &window_provider->size.y);
		window_provider->windowResized();
	}

	static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods)
	{
		auto window_provider = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS)
		{
			KeyCode code = KeyCode::Undefined;
			switch (key)
			{
				case GLFW_KEY_R:
					code = KeyCode::R;
					break;
			}
			window_provider->keyPressed(code);
		}
	}

	void setIcon()
	{
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
};