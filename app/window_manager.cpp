#include "window_manager.h"

WindowManager::WindowManager(glm::ivec2 size, std::string title)
{
	this->size = size;
	this->title = title;

	glfwSetErrorCallback(glfw_error_callback);
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	window = glfwCreateWindow(size.x, size.y, title.data(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSetWindowUserPointer(window, this);
	glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mousePosCallback);
	glfwSetScrollCallback(window, scrollCallback);
	glfwSetMouseButtonCallback(window, mouseButtonCallback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	setIcon();
}

glm::ivec2 WindowManager::getSize() const
{
	return size;
}

glm::vec2 WindowManager::getMousePos() const
{
	return mousePos;
}

void WindowManager::poolEvents()
{
	glfwPollEvents();
}

void WindowManager::waitForNoZero()
{
	glfwGetFramebufferSize(window, &size.x, &size.y);
	while (size.x == 0 || size.y == 0)
	{
		glfwGetFramebufferSize(window, &size.x, &size.y);
	}
}

bool WindowManager::isShouldClose()
{
	return glfwWindowShouldClose(window);
}

std::vector<const char*> WindowManager::getVulkanExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	return std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
}

VkSurfaceKHR WindowManager::createVkSurface(VkInstance& instance)
{
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
	return surface;
}

WindowManager::~WindowManager()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void WindowManager::setIcon()
{
	std::string filename = "icon.png";
	std::vector<unsigned char> buffer;
	std::vector<unsigned char> image;
	unsigned int w, h;

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

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

static void framebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	auto window_provider = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
	auto size = window_provider->getSize();
	glfwGetFramebufferSize(window, &size.x, &size.y);
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
			case GLFW_KEY_ESCAPE:
				code = KeyCode::ESC;
				break;
		}
		window_provider->keyPressed(code);
	}
}

static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	auto& wm = *reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (GLFW_PRESS == action)
		{
			wm.leftMousePressed = true;
			wm.leftMouseButtonPressed();
		}
		else if (GLFW_RELEASE == action)
		{
			wm.leftMousePressed = false;
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		if (GLFW_PRESS == action)
		{
			wm.rightMousePressed = true;
		}
		else if (GLFW_RELEASE == action)
		{
			wm.rightMousePressed = false;
		}
	}
}

static void mousePosCallback(GLFWwindow* window, double x_pos, double y_pos)
{
	auto& wm = *reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));

	glm::vec2 newPos = {
		static_cast<float>(x_pos),
		static_cast<float>(y_pos),
	};

	if (wm.rightMousePressed)
	{
		wm.pressedRightMouseButtonMoved(wm.mousePos - newPos);
	}

	wm.mousePos = { newPos };
}

static void scrollCallback(GLFWwindow* window, double x_offset, double y_offset)
{
	auto window_provider = reinterpret_cast<WindowManager*>(glfwGetWindowUserPointer(window));
	float y_off = static_cast<float>(y_offset);
	if (!mt::is0_f(y_off))
	{
		window_provider->mouseScroll(y_off);
	}
}
