#pragma once

#define GLFW_INCLUDE_VULKAN
#include "glm/glm.hpp"
#include "lodepng.h"
#include "math_tools.h"
#include <GLFW/glfw3.h>
#include <boost/signals2.hpp>
#include <cstdint>
#include <filesystem>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

enum KeyCode
{
	Undefined,
	ESC,
	R,
};

class WindowManager
{
	private:
	glm::ivec2 size{};
	std::string title{};
	GLFWwindow* window;

	public:
	glm::vec2 mousePos{};
	bool leftMousePressed;
	bool rightMousePressed;
	glm::vec2 lastMousePos;

	boost::signals2::signal<void()> windowResized{};
	boost::signals2::signal<void(KeyCode)> keyPressed{};
	boost::signals2::signal<void(float)> mouseScroll{};
	boost::signals2::signal<void()> leftMouseButtonPressed{};
	boost::signals2::signal<void(glm::vec2)> pressedRightMouseButtonMoved{};

	WindowManager(glm::ivec2 size, std::string title);

	glm::ivec2 getSize() const;
	glm::vec2 getMousePos() const;
	void poolEvents();
	void waitForNoZero();
	bool isShouldClose();
	std::vector<const char*> getVulkanExtensions();
	VkSurfaceKHR createVkSurface(VkInstance& instance);
	~WindowManager();

	private:
	void setIcon();
};

static void glfw_error_callback(int error, const char* description);
static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
static void keyCallback(GLFWwindow* window, int key, int scanCode, int action, int mods);
static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
static void mousePosCallback(GLFWwindow* window, double x_pos, double y_pos);
static void scrollCallback(GLFWwindow* window, double x_offset, double y_offset);