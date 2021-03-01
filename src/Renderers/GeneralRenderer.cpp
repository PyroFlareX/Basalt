#include "GeneralRenderer.h"


GeneralRenderer::GeneralRenderer(vn::Device* mainDevice, VkRenderPass* rpass)
{
	p_device = mainDevice;
	vn::vk::createCommandPool(*p_device, m_pool);
	m_renderpass = rpass;

	img.loadFromFile("res/container.jpg");

	m_renderlist.resize(50);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
	allocInfo.commandBufferCount = m_renderlist.size();

	if (vkAllocateCommandBuffers(p_device->getDevice(), &allocInfo, m_renderlist.data()) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to allocate command buffers!");
	}

	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Optional


	inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	

	inheritanceInfo.framebuffer = VK_NULL_HANDLE;

	beginInfo.pInheritanceInfo = &inheritanceInfo; // Optional

}

void GeneralRenderer::addInstance(vn::GameObject& entity)
{
	//Hack/Workaround to avoid cyclic dependency/init of the renderpass
	if (!firstrun)
	{
		inheritanceInfo.renderPass = *m_renderpass; //Renderpass
		firstrun = true;
	}

	m_queue.emplace_back(entity);
}

void GeneralRenderer::render(Camera& cam)
{
	
	

	for (uint8_t i = 0; i < m_queue.size(); ++i)
	{

		if (vkBeginCommandBuffer(m_renderlist.at(i), &beginInfo) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		vkCmdBindPipeline(m_renderlist.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS,
			*reinterpret_cast<VkPipeline*>(job.data[3]));

		vkCmdPushConstants(m_renderlist.at(i), *reinterpret_cast<VkPipelineLayout*>(job.data[8]),
			VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantsStruct), &pushconst);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(m_renderlist.at(i), 0, 1, &buffermesh->getAPIResource(), &offset);

		vkCmdBindIndexBuffer(m_renderlist.at(i), buffermesh->m_index, offset, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(m_renderlist.at(i), buffermesh->getNumElements(), 1, 0, 0, 0);


		if (vkEndCommandBuffer(m_renderlist.at(i)) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void GeneralRenderer::clearQueue()
{
	m_queue.clear();
	vkResetCommandPool(p_device->getDevice(), m_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
}

std::vector<VkCommandBuffer> GeneralRenderer::getRenderlists() const
{
	return m_renderlist;
}

GeneralRenderer::~GeneralRenderer()
{

}
