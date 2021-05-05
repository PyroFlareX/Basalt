#include "Renderer.h"

Renderer::Renderer(vn::Device* renderingDevice)
{
	device = renderingDevice;

	//Folding Scope
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = VK_FORMAT_B8G8R8A8_SRGB;
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

		if (vkCreateRenderPass(device->getDevice(), &renderPassInfo, nullptr, &renderpassdefault) != VK_SUCCESS) {
			throw std::runtime_error("failed to create render pass!");
		}
	}

	vn::vk::createCommandPool(*device, m_pool);

	m_primaryBuffers.resize(vn::vk::NUM_SWAPCHAIN_FRAMEBUFFERS);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = m_primaryBuffers.size();

	if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, m_primaryBuffers.data()) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}


	// Descriptor Pools
	VkDescriptorPoolCreateInfo descpoolinfo{};
	
	descpoolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descpoolinfo.pNext = nullptr;
	descpoolinfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

	VkDescriptorPoolSize descpoolsize[2] = {};
	descpoolsize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descpoolsize[0].descriptorCount = 1;
	
	descpoolsize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descpoolsize[1].descriptorCount = 1;

	descpoolinfo.pPoolSizes = &descpoolsize[0];
	descpoolinfo.poolSizeCount = 2;

	descpoolinfo.maxSets = 100;

	VkResult result = vkCreateDescriptorPool(device->getDevice(), &descpoolinfo, nullptr, &m_descpool);

	// Descriptor Sets
	VkDescriptorSetLayoutBinding setlayoutbinding[2] = {};
	setlayoutbinding[0].binding = 0;
	setlayoutbinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	setlayoutbinding[0].stageFlags = VK_SHADER_STAGE_ALL;
	setlayoutbinding[0].descriptorCount = 1;

	setlayoutbinding[1].binding = 1;
	setlayoutbinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	setlayoutbinding[1].stageFlags = VK_SHADER_STAGE_ALL;
	setlayoutbinding[1].descriptorCount = 1;
	

	VkDescriptorSetLayoutCreateInfo desclayoutinfo{};
	desclayoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	desclayoutinfo.pNext = nullptr;
	desclayoutinfo.bindingCount = 2; // 2;
	desclayoutinfo.pBindings = &setlayoutbinding[0];
	desclayoutinfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;

	VkDescriptorSetLayout desclayout;
	
	result = vkCreateDescriptorSetLayout(device->getDevice(), &desclayoutinfo, nullptr, &desclayout);

	VkDescriptorSetAllocateInfo descriptorAllocInfo{};
	descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorAllocInfo.descriptorPool = m_descpool;
	descriptorAllocInfo.descriptorSetCount = 1;
	descriptorAllocInfo.pSetLayouts = &desclayout;
	
	result = vkAllocateDescriptorSets(device->getDevice(), &descriptorAllocInfo, &m_descsetglobal);

	vn::asset_manager.pDescsetglobal = &m_descsetglobal;

	struct test
	{
		vn::mat4 proj;
		vn::mat4 view;
		vn::mat4 model;
	};

	test* uniformbufferthing = new test;

	// Descriptor Set Buffers:

	// Uniform buffer
	vn::vk::BufferDescription uniform;
	uniform.bufferType = vn::BufferUsage::UNIFORM_BUFFER;
	uniform.dev = device;
	uniform.size = 192;
	uniform.stride = 64;
	uniform.bufferData = uniformbufferthing;

	m_descriptorBuffers.emplace_back(new vn::vk::Buffer(uniform));

	m_descriptorBuffers.at(0)->uploadBuffer();


	m_generalRenderer = new GeneralRenderer(renderingDevice, &renderpassdefault, desclayout);
}

void Renderer::drawObject(vn::GameObject& entity)
{
	m_generalRenderer->addInstance(entity);
}

void Renderer::doCompute()
{

}

void Renderer::render(Camera& cam)
{
	//Main Pass
	void* params[] = { this, &cam };
	

	Job generalRender = jobSystem.createJob([](Job job)
		{
			static_cast<Renderer*>(job.data[0])->m_generalRenderer->render(*static_cast<Camera*>(job.data[1]));
		}, params);

	
	//m_generalRenderer->render(cam);

	jobSystem.schedule(generalRender);

	// TODO: Add way to update the content of a buffer before pushing to GPU, purpose being MVP matrices
	pushGPUData();

	jobSystem.wait();
}

void Renderer::finish(vn::vk::FramebufferData& fbo)
{
	vkResetCommandPool(device->getDevice(), m_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
	//Second Pass
	auto& renderLists = m_generalRenderer->getRenderlists(); // The secondary command buffers


	for (size_t i = 0; i < m_primaryBuffers.size(); ++i) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

		if (vkBeginCommandBuffer(m_primaryBuffers.at(i), &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = renderpassdefault;
		renderPassInfo.framebuffer = fbo.handle[i];

		renderPassInfo.renderArea.offset = { 0, 0 };
		
		VkExtent2D extent;
		extent.height = (int)fbo.size.y;
		extent.width = (int)fbo.size.x ;

		renderPassInfo.renderArea.extent = extent;

		//VkClearDepthStencilValue depthClear = {};
		VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
		//VK_SUBPASS_CONTENTS_INLINE //VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
		vkCmdBeginRenderPass(m_primaryBuffers.at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		
		vkCmdExecuteCommands(m_primaryBuffers.at(i), renderLists.size(), renderLists.data());

		vkCmdEndRenderPass(m_primaryBuffers.at(i));

		if (vkEndCommandBuffer(m_primaryBuffers.at(i)) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}

	device->submitWork(m_primaryBuffers);
}

void Renderer::clearQueue()
{
	m_generalRenderer->clearQueue();
}

Renderer::~Renderer()
{
	//dtor
	vkDestroyRenderPass(device->getDevice(), renderpassdefault, nullptr);
	vkDestroyCommandPool(device->getDevice(), m_pool, nullptr);
	delete m_generalRenderer;
}

void Renderer::pushGPUData()
{

	//Buffer Writing Info
	VkDescriptorBufferInfo bufferInfo1{};
	bufferInfo1.buffer = m_descriptorBuffers.at(0)->getAPIResource();
	bufferInfo1.offset = 0;
	bufferInfo1.range = 16;

	//Image Writing Info
	VkDescriptorImageInfo imageinfo1{};

	/*auto& texture = vn::asset_manager.getTexture(0);

	imageinfo1.imageView = texture.getAPITextureInfo().imgviewvk;
	imageinfo1.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	imageinfo1.sampler = texture.getAPITextureInfo().sampler;
*/
	auto textures = vn::asset_manager.getTextures();

	std::vector<VkDescriptorImageInfo> imageinfo;

	for(int i = 0; i < textures.size(); ++i)
	{
		VkDescriptorImageInfo imginfo;
		imginfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imginfo.imageView = textures[i].imgviewvk;
		imginfo.sampler = textures[i].sampler;

		imageinfo.emplace_back(imginfo);
	}

	


	//Writing Info
	VkWriteDescriptorSet descWrite[2] = {};
	descWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrite[0].dstSet = m_descsetglobal;
	descWrite[0].dstBinding = 0;
	descWrite[0].dstArrayElement = 0; // Double check later
	descWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descWrite[0].descriptorCount = 1;
	descWrite[0].pBufferInfo = &bufferInfo1;
	
	descWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrite[1].dstSet = m_descsetglobal;
	descWrite[1].dstBinding = 1;
	descWrite[1].dstArrayElement = 0; // Double check later
	descWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descWrite[1].descriptorCount = 1;
	descWrite[1].pImageInfo = &imageinfo.at(0);
	

	
	// Double check other values later

	vkUpdateDescriptorSets(device->getDevice(), 2, &descWrite[0], 0, nullptr);
}