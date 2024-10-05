#pragma once

#include "data_provider.cpp"
#include "graphic_engine_builders.cpp"
#include "validation_manager.cpp"
#include "vertex.cpp"
#include "vulkan_device.cpp"
#include "vulkan_tools.cpp"
#include "window_provider.cpp"
#include <algorithm>
#include <boost/bind/bind.hpp>
#include <cstdint>
#include <cstring>
#include <fstream>
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

	WindowProvider* windowProvider;
	DataProvider* dataProvider;

	VkInstance instance;
	ValidationManager validationManager;
	VkSurfaceKHR surface;

	VulkanDevice vulkanDevice;

	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> swapChainFramebuffers;

	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;

	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	std::vector<VkFence> inFlightFences;

	uint32_t currentFrame = 0;
	bool swapChainRecreationPending = false;

	std::vector<bool> vertexBuffersRecreationPending;
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceMemory> vertexBufferMemories;

	GraphicEngine(WindowProvider* windowProvider, DataProvider* dataProvider, GraphicsEngineConfig config)
	{
		this->config = config;
		this->windowProvider = windowProvider;
		windowProvider->windowResized.connect(boost::bind(&GraphicEngine::pendSwapChainRecreation, this));
		this->dataProvider = dataProvider;
		dataProvider->prepareDataToDraw();
		initVulkan();
	}

	void draw()
	{
		drawFrame();
	}

	~GraphicEngine()
	{
		vulkanDevice.waitIdle();
		cleanup();
	}

	private:
	void pendSwapChainRecreation()
	{
		swapChainRecreationPending = true;
	}

	void initVulkan()
	{
		// Create Instance
		InstanceBuilder instanceBuilder{};
		instanceBuilder.config = config;
		instanceBuilder.validationManager = &validationManager;
		instanceBuilder.windowProvider = windowProvider;
		instanceBuilder.build(&instance);

		// Create Surface
		surface = windowProvider->createVkSurface(instance);

		// VkPhysical device and VkDevice
		vulkanDevice.instance = instance;
		vulkanDevice.surface = surface;
		vulkanDevice.validationManager = &validationManager;
		vulkanDevice.init();

		createSwapChain(); // recreating
		createImageViews(); // recreating
		createRenderPass();
		createGraphicsPipeline();
		createFramebuffers(); // recreating
		createCommandPool();
		createVertexBuffers(); // recreating
		createCommandBuffer();
		createSyncObjects();
	}

	void recreateSwapChain()
	{
		windowProvider->waitForNoZero();
		vulkanDevice.waitIdle();

		cleanupSwapChain();

		createSwapChain();
		createImageViews();
		createFramebuffers();
	}

	void cleanupSwapChain()
	{
		for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
		{
			vkDestroyFramebuffer(vulkanDevice.logicalDevice, swapChainFramebuffers[i], nullptr);
		}

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			vkDestroyImageView(vulkanDevice.logicalDevice, swapChainImageViews[i], nullptr);
		}

		vkDestroySwapchainKHR(vulkanDevice.logicalDevice, swapChain, nullptr);
	}

	void cleanupVertexBuffer(size_t i)
	{
		vkDestroyBuffer(vulkanDevice.logicalDevice, vertexBuffers[i], nullptr);
		vkFreeMemory(vulkanDevice.logicalDevice, vertexBufferMemories[i], nullptr);
	}

	void cleanup()
	{
		cleanupSwapChain();

		for (size_t i = 0; i < config.maxFramesInFlight; i++)
		{
			cleanupVertexBuffer(i);
		}

		vkDestroyPipeline(vulkanDevice.logicalDevice, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(vulkanDevice.logicalDevice, pipelineLayout, nullptr);

		vkDestroyRenderPass(vulkanDevice.logicalDevice, renderPass, nullptr);

		for (size_t i = 0; i < config.maxFramesInFlight; i++)
		{
			vkDestroySemaphore(vulkanDevice.logicalDevice, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(vulkanDevice.logicalDevice, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(vulkanDevice.logicalDevice, inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(vulkanDevice.logicalDevice, commandPool, nullptr);

		vulkanDevice.cleanup();
		validationManager.cleanup(instance);

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);
	}

	void drawFrame()
	{
		vkWaitForFences(vulkanDevice.logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(vulkanDevice.logicalDevice, swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			recreateSwapChain();
			return;
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		if (dataProvider->prepareDataToDraw())
		{
			for (size_t i = 0; i < config.maxFramesInFlight; i++)
			{
				vertexBuffersRecreationPending[i] = true;
			}
		}

		if (vertexBuffersRecreationPending[currentFrame])
		{
			recreateVertexBuffer(currentFrame);
		}
		copyBufferMemory(currentFrame);

		vkResetFences(vulkanDevice.logicalDevice, 1, &inFlightFences[currentFrame]);

		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		recordCommandBuffer(commandBuffers[currentFrame], imageIndex);

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK(vkQueueSubmit(vulkanDevice.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]));

		VkSwapchainKHR swapChains[] = { swapChain };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(vulkanDevice.presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || swapChainRecreationPending)
		{
			swapChainRecreationPending = false;
			recreateSwapChain();
			recreateVertexBuffer(currentFrame);
			copyBufferMemory(currentFrame);
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		currentFrame = (currentFrame + 1) % config.maxFramesInFlight;
	}

	void createSyncObjects()
	{
		imageAvailableSemaphores.resize(config.maxFramesInFlight);
		renderFinishedSemaphores.resize(config.maxFramesInFlight);
		inFlightFences.resize(config.maxFramesInFlight);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < config.maxFramesInFlight; i++)
		{
			VK_CHECK(vkCreateSemaphore(vulkanDevice.logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
			VK_CHECK(vkCreateSemaphore(vulkanDevice.logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
			VK_CHECK(vkCreateFence(vulkanDevice.logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]));
		}
	}

	void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderPass;
		renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = swapChainExtent;
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(swapChainExtent.width);
		viewport.height = static_cast<float>(swapChainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = swapChainExtent;
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

		VkBuffer vertexBuffers[] = { this->vertexBuffers[currentFrame] };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

		vkCmdDraw(commandBuffer, dataProvider->verticesSize(), dataProvider->triangleCount(), 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		VK_CHECK(vkEndCommandBuffer(commandBuffer));
	}

	void createVertexBuffers()
	{
		vertexBuffersRecreationPending.resize(config.maxFramesInFlight);
		vertexBuffers.resize(config.maxFramesInFlight);
		vertexBufferMemories.resize(config.maxFramesInFlight);

		for (size_t i = 0; i < config.maxFramesInFlight; i++)
		{
			createVertexBuffer(i);
			copyBufferMemory(i);
			vertexBuffersRecreationPending[i] = false;
		}
	}

	void recreateVertexBuffer(size_t i)
	{
		cleanupVertexBuffer(i);
		createVertexBuffer(i);
		vertexBuffersRecreationPending[i] = false;
	}

	void createVertexBuffer(size_t i)
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = dataProvider->getVkSize();
		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VK_CHECK(vkCreateBuffer(vulkanDevice.logicalDevice, &bufferInfo, nullptr, &vertexBuffers[i]));

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(vulkanDevice.logicalDevice, vertexBuffers[i], &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		VK_CHECK(vkAllocateMemory(vulkanDevice.logicalDevice, &allocInfo, nullptr, &vertexBufferMemories[i]));

		vkBindBufferMemory(vulkanDevice.logicalDevice, vertexBuffers[i], vertexBufferMemories[i], 0);
	}

	void copyBufferMemory(size_t idx)
	{
		void* data;
		vkMapMemory(vulkanDevice.logicalDevice, vertexBufferMemories[idx], 0, dataProvider->getVkSize(), 0, &data);
		memcpy(data, dataProvider->getDataPointer(), (size_t)dataProvider->getVkSize());
		vkUnmapMemory(vulkanDevice.logicalDevice, vertexBufferMemories[idx]);
	}

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const
	{
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(vulkanDevice.physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
		{
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
			{
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void createCommandBuffer()
	{
		commandBuffers.resize(config.maxFramesInFlight);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

		VK_CHECK(vkAllocateCommandBuffers(vulkanDevice.logicalDevice, &allocInfo, commandBuffers.data()));
	}

	void createCommandPool()
	{
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = vulkanDevice.queueFamilyIndices.graphicsFamily.value();

		VK_CHECK(vkCreateCommandPool(vulkanDevice.logicalDevice, &poolInfo, nullptr, &commandPool));
	}

	void createFramebuffers()
	{
		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			VkImageView attachments[] = {
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments;
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			VK_CHECK(vkCreateFramebuffer(vulkanDevice.logicalDevice, &framebufferInfo, nullptr, &swapChainFramebuffers[i]))
		}
	}

	void createRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VK_CHECK(vkCreateRenderPass(vulkanDevice.logicalDevice, &renderPassInfo, nullptr, &renderPass));
	}

	void createGraphicsPipeline()
	{
		auto vertShaderCode = readFile("vert.spv");
		auto fragShaderCode = readFile("frag.spv");

		VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
		VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		auto bindingDescription = Vertex::getBindingDescription();
		auto attributeDescriptions = Vertex::getAttributeDescriptions();

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;

		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pushConstantRangeCount = 0;

		VK_CHECK(vkCreatePipelineLayout(vulkanDevice.logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout));

		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = pipelineLayout;
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
		pipelineInfo.basePipelineIndex = -1;

		VK_CHECK(vkCreateGraphicsPipelines(vulkanDevice.logicalDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline));

		vkDestroyShaderModule(vulkanDevice.logicalDevice, fragShaderModule, nullptr);
		vkDestroyShaderModule(vulkanDevice.logicalDevice, vertShaderModule, nullptr);
	}

	VkShaderModule createShaderModule(const std::vector<char>& code) const
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		VK_CHECK(vkCreateShaderModule(vulkanDevice.logicalDevice, &createInfo, nullptr, &shaderModule));
		return shaderModule;
	}

	void createImageViews()
	{
		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;

			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;

			VK_CHECK(vkCreateImageView(vulkanDevice.logicalDevice, &createInfo, nullptr, &swapChainImageViews[i]));
		}
	}

	void createSwapChain()
	{
		SwapChainSupportDetails swapChainSupport = vulkanDevice.swapChainSupport;

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = vulkanDevice.queueFamilyIndices;
		uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		VK_CHECK(vkCreateSwapchainKHR(vulkanDevice.logicalDevice, &createInfo, nullptr, &swapChain));

		vkGetSwapchainImagesKHR(vulkanDevice.logicalDevice, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(vulkanDevice.logicalDevice, swapChain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) const
	{
		if (capabilities.currentExtent.width != (std::numeric_limits<uint32_t>::max)())
		{
			return capabilities.currentExtent;
		}
		else
		{
			auto extendSize = windowProvider->getSize();

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(extendSize.x),
				static_cast<uint32_t>(extendSize.y)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

			return actualExtent;
		}
	}

	static std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open())
		{
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}
};