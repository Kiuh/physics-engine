#include "graphic_engine.h"

GraphicEngine::GraphicEngine(WindowManager* windowManager, DataManager* dataManager, GraphicsEngineConfig config)
{
	this->config = config;
	this->windowManager = windowManager;
	this->dataManager = dataManager;
	initManagers();
	initVulkan();
}

void GraphicEngine::draw()
{
	drawFrame();
}

GraphicEngine::~GraphicEngine()
{
	device.waitIdle();
	cleanup();
}

void GraphicEngine::pendSwapChainRecreation()
{
	swapChainRecreationPending = true;
}

void GraphicEngine::pendBuffersRecreation()
{
	for (size_t i = 0; i < bufferRecreationPending.size(); i++)
	{
		bufferRecreationPending[i] = true;
	}
}

void GraphicEngine::initManagers()
{
	windowManager->windowResized.connect(boost::bind(&GraphicEngine::pendSwapChainRecreation, this));
	dataManager->dataStructureChanged.connect(boost::bind(&GraphicEngine::pendBuffersRecreation, this));
}

void GraphicEngine::initVulkan()
{
	// Create Instance
	InstanceBuilder instanceBuilder{};
	instanceBuilder.config = config;
	instanceBuilder.validationManager = &validationManager;
	instanceBuilder.windowManager = windowManager;
	instanceBuilder.build(instance);

	// Create Surface
	surface = windowManager->createVkSurface(instance);

	// VkPhysical device and VkDevice
	device.instance = instance;
	device.surface = surface;
	device.validationManager = &validationManager;
	device.config = config;
	device.init();

	// VkSwapchainKHR
	swapchain.surface = surface;
	swapchain.device = &device;
	swapchain.windowManager = windowManager;
	swapchain.create();

	shaderManager.vulkanDevice = &device;
	createRenderPass();
	createGraphicsPipeline();

	// Frame buffers
	swapchain.addFrameBuffers(&renderPass);

	createBuffers();
	createSyncObjects();
}

void GraphicEngine::waitAndRecreateSwapChain()
{
	windowManager->waitForNoZero();
	device.waitIdle();
	swapchain.recreate();
}

void GraphicEngine::cleanupBuffer(size_t idx)
{
	vertexBuffers[idx].cleanup();
	indexBuffers[idx].cleanup();
}

void GraphicEngine::cleanup()
{
	swapchain.cleanup();

	for (size_t i = 0; i < config.maxFramesInFlight; i++)
	{
		cleanupBuffer(i);
	}

	vkDestroyPipeline(device.logicalDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(device.logicalDevice, pipelineLayout, nullptr);

	vkDestroyRenderPass(device.logicalDevice, renderPass, nullptr);

	for (size_t i = 0; i < config.maxFramesInFlight; i++)
	{
		vkDestroySemaphore(device.logicalDevice, renderFinishedSemaphores[i], nullptr);
		vkDestroySemaphore(device.logicalDevice, imageAvailableSemaphores[i], nullptr);
		vkDestroyFence(device.logicalDevice, inFlightFences[i], nullptr);
	}

	device.cleanup();
	validationManager.cleanup(instance);

	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyInstance(instance, nullptr);
}

void GraphicEngine::drawFrame()
{
	vkWaitForFences(device.logicalDevice, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(
		device.logicalDevice,
		swapchain.instance,
		UINT64_MAX,
		imageAvailableSemaphores[currentFrame],
		VK_NULL_HANDLE,
		&imageIndex
	);

	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		waitAndRecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("failed to acquire swap chain image!");
	}

	if (bufferRecreationPending[currentFrame])
	{
		cleanupBuffer(currentFrame);
		createBuffer(currentFrame);
		bufferRecreationPending[currentFrame] = false;
	}
	dataManager->recalculateVertexes();
	vertexBuffers[currentFrame].copyNewData();

	vkResetFences(device.logicalDevice, 1, &inFlightFences[currentFrame]);

	vkResetCommandBuffer(device.commandBuffers[currentFrame], 0);
	recordCommandBuffer(device.commandBuffers[currentFrame], imageIndex);

	VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &device.commandBuffers[currentFrame];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	VK_CHECK(vkQueueSubmit(device.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]));

	VkSwapchainKHR swapChains[] = { swapchain.instance };

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	result = vkQueuePresentKHR(device.presentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || swapChainRecreationPending)
	{
		swapChainRecreationPending = false;
		waitAndRecreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("failed to present swap chain image!");
	}

	currentFrame = (currentFrame + 1) % config.maxFramesInFlight;
}

void GraphicEngine::createSyncObjects()
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
		VK_CHECK(vkCreateSemaphore(device.logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]));
		VK_CHECK(vkCreateSemaphore(device.logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]));
		VK_CHECK(vkCreateFence(device.logicalDevice, &fenceInfo, nullptr, &inFlightFences[i]));
	}
}

void GraphicEngine::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
{
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));

	VkClearValue clearColor = { {{0.01f, 0.01f, 0.01f, 1.0f}} };

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderPass;
	renderPassInfo.framebuffer = swapchain.framebuffers[imageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = swapchain.extent;
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(swapchain.extent.width);
	viewport.height = static_cast<float>(swapchain.extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = swapchain.extent;
	vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

	VkBuffer vertexBuffers[] = { this->vertexBuffers[currentFrame].buffer };
	VkDeviceSize offsets[] = { 0 };

	vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
	vkCmdBindIndexBuffer(commandBuffer, indexBuffers[currentFrame].buffer, 0, VK_INDEX_TYPE_UINT16);

	uint32_t indexCount = dataManager->indexes.size();
	uint32_t triangleCount = indexCount / 3;
	vkCmdDrawIndexed(commandBuffer, indexCount, triangleCount, 0, 0, 0);

	vkCmdEndRenderPass(commandBuffer);

	VK_CHECK(vkEndCommandBuffer(commandBuffer));
}

void GraphicEngine::createBuffers()
{
	vertexBuffers.resize(config.maxFramesInFlight);
	indexBuffers.resize(config.maxFramesInFlight);
	bufferRecreationPending = std::vector<bool>(config.maxFramesInFlight, false);

	for (size_t i = 0; i < config.maxFramesInFlight; i++)
	{
		createBuffer(i);
	}
}

void GraphicEngine::createBuffer(size_t idx)
{
	VulkanBufferBuilder vertexBuilder{};
	vertexBuilder.device = &device;
	vertexBuilder.usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	vertexBuilder.bufferSize = dataManager->vertices.getVkSize();
	vertexBuilder.data_mutex = &dataManager->data_mutex;
	vertexBuilder.sourceData = dataManager->vertices.getDataPointer();
	vertexBuffers[idx] = vertexBuilder.build();

	VulkanBufferBuilder indexBuilder{};
	indexBuilder.device = &device;
	indexBuilder.usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
	indexBuilder.bufferSize = dataManager->indexes.getVkSize();
	indexBuilder.data_mutex = &dataManager->data_mutex;
	indexBuilder.sourceData = dataManager->indexes.getDataPointer();
	indexBuffers[idx] = indexBuilder.build();
	indexBuffers[idx].copyNewData();
}

void GraphicEngine::createRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = swapchain.imageFormat;
	colorAttachment.samples = device.msaaSamples;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription colorAttachmentResolve{};
	colorAttachmentResolve.format = swapchain.imageFormat;
	colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference colorAttachmentResolveRef{};
	colorAttachmentResolveRef.attachment = 1;
	colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pResolveAttachments = &colorAttachmentResolveRef;

	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, colorAttachmentResolve };

	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	VK_CHECK(vkCreateRenderPass(device.logicalDevice, &renderPassInfo, nullptr, &renderPass));
}

void GraphicEngine::createGraphicsPipeline()
{
	VkShaderModule vertShaderModule = shaderManager.createShaderModule("vert.spv");
	VkShaderModule fragShaderModule = shaderManager.createShaderModule("frag.spv");

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
	rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
	//rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling{};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = device.msaaSamples;

	VkPipelineColorBlendAttachmentState colorBlendAttachment{};
	colorBlendAttachment.colorWriteMask =
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
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

	VK_CHECK(vkCreatePipelineLayout(device.logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout));

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

	VK_CHECK(
		vkCreateGraphicsPipelines(
			device.logicalDevice,
			VK_NULL_HANDLE,
			1,
			&pipelineInfo,
			nullptr,
			&graphicsPipeline
		)
	);

	vkDestroyShaderModule(device.logicalDevice, fragShaderModule, nullptr);
	vkDestroyShaderModule(device.logicalDevice, vertShaderModule, nullptr);
}