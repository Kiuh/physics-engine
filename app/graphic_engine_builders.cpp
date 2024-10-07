#pragma once

#include "validation_manager.cpp"
#include "vulkan_tools.cpp"
#include "window_manager.cpp"
#include <vulkan/vulkan_core.h>

struct InstanceBuilder
{
	ValidationManager* validationManager;
	GraphicsEngineConfig config;
	WindowManager* windowManager;

	void build(VkInstance& instance)
	{
		validationManager->init(config);

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Physics Simulation";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		std::vector<const char*> instance_extensions{};

		auto win_ext = windowManager->getVulkanExtensions();
		instance_extensions.insert(instance_extensions.end(), win_ext.begin(), win_ext.end());

		auto debug_ext = validationManager->getDebugMessengerExtensions(config);
		instance_extensions.insert(instance_extensions.end(), debug_ext.begin(), debug_ext.end());

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(instance_extensions.size());
		createInfo.ppEnabledExtensionNames = instance_extensions.data();
		validationManager->addMessengerToInstance(createInfo);

		VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));

		validationManager->setupDebugMessenger(instance);
	}
};