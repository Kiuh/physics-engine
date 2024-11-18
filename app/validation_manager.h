#pragma once

#include "vulkan_tools.h"
#include <cstdint>
#include <iostream>
#include <magic_enum.hpp>
#include <stdexcept>
#include <string.h>
#include <vector>
#include <vulkan/vk_platform.h>
#include <vulkan/vulkan_core.h>

bool replace(std::string& str, const std::string& from, const std::string& to);
VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData
);

struct ValidationManager
{
	private:
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_KHRONOS_validation",
	};
	VkDebugUtilsMessengerEXT debugMessenger{};
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	GraphicsEngineConfig config;

	public:
	void init(GraphicsEngineConfig config);
	std::vector<const char*> getDebugMessengerExtensions(GraphicsEngineConfig config);
	void addMessengerToInstance(VkInstanceCreateInfo& createInfo);
	void addMessengerToDevice(VkDeviceCreateInfo& createInfo);
	void setupDebugMessenger(VkInstance& instance);
	void cleanup(VkInstance& instance);

	private:
	bool checkValidationLayerSupport();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	VkResult CreateDebugUtilsMessengerEXT(
		VkInstance& instance,
		const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator,
		VkDebugUtilsMessengerEXT* pDebugMessenger
	);
	void DestroyDebugUtilsMessengerEXT(
		VkInstance& instance,
		VkDebugUtilsMessengerEXT debugMessenger,
		const VkAllocationCallbacks* pAllocator
	);
};