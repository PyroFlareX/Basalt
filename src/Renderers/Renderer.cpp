#include "Renderer.h"

Renderer::Renderer(vn::Device* renderingDevice)
{
	device = renderingDevice;

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

	m_primaryBuffers.resize(3);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = m_primaryBuffers.size();

	if (vkAllocateCommandBuffers(device->getDevice(), &allocInfo, m_primaryBuffers.data()) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	m_generalRenderer = new GeneralRenderer(renderingDevice, &renderpassdefault);
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
	static void** params = new void*[2]{ this, &cam };
	

	Job generalRender = jobSystem.createJob([](Job job)
		{
			static_cast<Renderer*>(job.data[0])->m_generalRenderer->render(*static_cast<Camera*>(job.data[1]));
		}, params);

	//jobSystem.schedule(generalRender);
	m_generalRenderer->render(cam);

	jobSystem.wait();
	std::cout << "Render:::: \n";
}

void Renderer::finish(vn::vk::FramebufferData& fbo)
{
	vkResetCommandPool(device->getDevice(), m_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
	//Second Pass
	auto renderLists = m_generalRenderer->getRenderlists();

	std::cout << "Finish:::: \n";

	for (size_t i = 0; i < 3/*m_primaryBuffers.size()*/; ++i) {
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

		VkExtent2D extent = {fbo.size.y, fbo.size.x};

		renderPassInfo.renderArea.extent = extent;

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
	std::cout << "SUMBIT:::: \n";
	device->submitWork(m_primaryBuffers);

	//clearQueue();
}

void Renderer::clearQueue()
{
	m_generalRenderer->clearQueue();
}

Renderer::~Renderer()
{
    //dtor
}
