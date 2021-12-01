#include "GeneralRenderer.h"

#include <GPU/Vulkan/PipelineBuilder.h>

GeneralRenderer::GeneralRenderer(bs::Device* mainDevice, VkRenderPass& rpass, VkDescriptorSetLayout desclayout)	: 
	p_device(mainDevice), m_renderpass(rpass)
{
	bs::vk::createCommandPool(*p_device, m_pool);

	//Renderlists for secondary cmd buffers
	m_renderlist.resize(2);

	const VkCommandBufferAllocateInfo allocInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = m_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		.commandBufferCount = (u32)m_renderlist.size(),
	};

	if(vkAllocateCommandBuffers(p_device->getDevice(), &allocInfo, m_renderlist.data()) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	//Cmd buffer Inheritance info init
	m_inheritanceInfo = VkCommandBufferInheritanceInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		.pNext = nullptr,
		.renderPass = m_renderpass,
		.subpass = 0,
		// Set to Null because the finish render function has the target frame buffer to render to
		.framebuffer = VK_NULL_HANDLE,
		.occlusionQueryEnable = 0,
		.queryFlags = 0,
		.pipelineStatistics = 0
	};

	//Cmd buffer starting info
	m_beginInfo = VkCommandBufferBeginInfo 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
		.pInheritanceInfo = &m_inheritanceInfo,
	};
	
	bs::vk::GraphicsPipelineBuilder graphicsPipelineBuilder(p_device, m_renderpass, desclayout);
	graphicsPipelineBuilder.addVertexShader("res/Shaders/vert.spv");
	graphicsPipelineBuilder.addFragmentShader("res/Shaders/frag.spv");
	graphicsPipelineBuilder.setDrawMode(bs::vk::DrawMode::FILL);
	graphicsPipelineBuilder.setRasterizingData(false, true);
	graphicsPipelineBuilder.setPushConstantSize(sizeof(PushConstantsStruct));
	graphicsPipelineBuilder.useVertexDescription(bs::vk::getVertexDescription());
	
	graphicsPipelineBuilder.build();
	graphicsPipelineBuilder.getResults(m_genericPipeline, m_pipelineLayout);
}

GeneralRenderer::~GeneralRenderer()
{
	vkDestroyPipelineLayout(p_device->getDevice(), m_pipelineLayout, nullptr);
	vkDestroyPipeline(p_device->getDevice(), m_genericPipeline, nullptr);
	vkDestroyCommandPool(p_device->getDevice(), m_pool, nullptr);
}

void GeneralRenderer::addInstance(const bs::GameObject& entity)
{
	m_queue.emplace_back(entity);
}

void GeneralRenderer::render(Camera& cam)
{
	PushConstantsStruct pushconst 
	{
		.textureids = {
			2,	//first is texture
			1,	//second is normals
			0,	//third is ???
			0	//forth is ???
		},
	};

	//For Dynamic State
	const VkExtent2D extent
	{
		.width = bs::vk::viewportwidth,
		.height = bs::vk::viewportheight,
	};

	const VkViewport viewport
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = (float)extent.width,
		.height = (float)extent.height,
		.minDepth = 0.0f,
		.maxDepth = 1.0f,
	};

	const VkRect2D scissor
	{
		.offset = { 0, 0 },
		.extent = extent,
	};
	
	// STUPID WORKAROUND THING : THIS STARTS THE RECORDING FOR THE IMGUI SECONDARY
	// The idea is thet I reserve the 0th index of the secondary cmd buffers for imgui rendering
	if (vkBeginCommandBuffer(m_renderlist.at(0), &m_beginInfo) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to begin recording command buffer!");
	}
	
	for(auto i = 0; i < m_queue.size(); ++i)
	{
		auto& cmd_buf = m_renderlist.at(i + 1);
		if (vkBeginCommandBuffer(cmd_buf, &m_beginInfo) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}
		
		vkCmdBindPipeline(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_genericPipeline);
		//Dynamic Pipeline State
		vkCmdSetViewport(cmd_buf, 0, 1, &viewport);
		vkCmdSetScissor(cmd_buf, 0, 1, &scissor);

		vkCmdBindDescriptorSets(cmd_buf, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, 
			bs::asset_manager->pDescsetglobal, 0, nullptr);
		

		pushconst.textureids = bs::vec4		//uncomment when it actually becomes necessary to render >1 object that have different textures
		{
			m_queue.at(i).material.texture_id,	//first is texture
			m_queue.at(i).material.normal_id,	//second is normals
			0,	//third is ???
			0	//forth is ???
		};
		
		vkCmdPushConstants(cmd_buf, m_pipelineLayout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantsStruct), &pushconst);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmd_buf, 0, 1, &bs::asset_manager->getModel(std::move(m_queue.at(i).model_id)).getVertexBuffer()->getAPIResource(), &offset);
		
		vkCmdBindIndexBuffer(cmd_buf, bs::asset_manager->getModel(std::move(m_queue.at(i).model_id)).getIndexBuffer()->getAPIResource(), offset, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(cmd_buf, bs::asset_manager->getModel(std::move(m_queue.at(i).model_id)).getIndexBuffer()->getNumElements(), 1, 0, 0, 0);

		if(vkEndCommandBuffer(cmd_buf) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to record command buffer!");
		}
	}
}

void GeneralRenderer::executeCommands(VkCommandBuffer cmd)
{
	if(m_queue.size() > 0)
	{
		vkCmdExecuteCommands(cmd, m_queue.size(), m_renderlist.data());
	}
}

void GeneralRenderer::clearQueue()
{
	m_queue.clear();
	vkResetCommandPool(p_device->getDevice(), m_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
}

// Get list of secondary cmd buffers
std::vector<VkCommandBuffer> GeneralRenderer::getRenderlists()
{
	std::vector<VkCommandBuffer> cur_list = m_renderlist;
	cur_list.resize(1 + m_queue.size());
	return cur_list;
}
