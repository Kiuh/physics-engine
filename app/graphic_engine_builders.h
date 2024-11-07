#pragma once

#include "validation_manager.h"
#include "vulkan_tools.h"
#include "window_manager.h"
#include <vulkan/vulkan_core.h>

struct InstanceBuilder
{
	ValidationManager* validationManager;
	GraphicsEngineConfig config;
	WindowManager* windowManager;

	void build(VkInstance& instance);
};