#pragma once

#include "data_manager.h"
#include "graphic_engine_builders.h"
#include "shader_manager.h"
#include "validation_manager.h"
#include "vertex.h"
#include "vma_usage.h"
#include "vulkan_buffer.h"
#include "vulkan_device.h"
#include "vulkan_swapchain.h"
#include "vulkan_tools.h"
#include "window_manager.h"

#include <algorithm>
#include <boost/bind/bind.hpp>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>
#include <iosfwd>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string>
#include <vector>
#include <vulkan/vulkan_core.h>

class GraphicEngine
{
	public:
	GraphicsEngineConfig config;

	WindowManager* windowManager;
	DataManager* dataManager;

	VkInstance instance;
	ValidationManager validationManager;
	VkSurfaceKHR surface;

	VulkanDevice device;
	VulkanSwapchain swapchain;
	VmaAllocator vmaAllocator;

	ShaderManager shaderManager;

	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	VkDescriptorPool uiDescriptorPool{};
	VkRenderPass uiRenderPass{};

	uint32_t currentFrame = 0;
	bool swapChainRecreationPending = false;
	std::vector<bool> bufferRecreationPending{};

	std::vector<VulkanBuffer> vertexBuffers;
	std::vector<VulkanBuffer> indexBuffers;

	GraphicEngine(WindowManager* windowManager, DataManager* dataManager, GraphicsEngineConfig config);
	void draw();
	~GraphicEngine();

	private:
	void initManagers();

	void initVulkan();
	void createSyncObjects();
	void createBuffers();
	void createBuffer(size_t idx);
	void createRenderPass();
	void createGraphicsPipeline();

	void initUI();
	void buildUI();
	void createUIDescriptorPool();
	void createUIRenderPass();
	void recordUICommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void cleanupUI() const;

	void drawFrame();
	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);
	void waitAndRecreateSwapChain();
	void cleanupBuffer(size_t idx);
	void pendSwapChainRecreation();
	void pendBuffersRecreation();
	void cleanup();
};