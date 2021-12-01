#include "GeneralRenderer.h"


GeneralRenderer::GeneralRenderer(bs::Device* mainDevice, VkRenderPass* rpass, VkDescriptorSetLayout desclayout)
{
	p_device = mainDevice;
	bs::vk::createCommandPool(*p_device, m_pool);
	m_renderpass = rpass;

	// Mesh
	/*std::string flatplane("flatplane");
	void* data[] = { p_device, &flatplane };

	//Num Models
	short numModels = 1;
	
	//Create a job for each model to load, the job loads the model and adds it to the asset manager
	for(int i = 1; i <= numModels; ++i)
	{
		Job load = jobSystem.createJob([i](Job j)
		{
			int index = i;
			auto* r = j.data[index];
			// will crash if pointer is uninitialized
			std::string s(*static_cast<std::string*>(r));

			bs::Mesh m = bs::loadMeshFromObj("res/Models/" + s + ".obj");
			auto* model = new bs::vk::Model(m, static_cast<bs::Device*>(j.data[0]));
			bs::asset_manager->addModel(*model, std::move(s));
		}, data);
		
		jobSystem.schedule(load);
	}*/

	//Renderlists for secondary cmd buffers
	m_renderlist.resize(10);

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
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

	inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
	
	// Set to Null because the finish render function has the target frame buffer to render to
	inheritanceInfo.framebuffer = VK_NULL_HANDLE;
	beginInfo.pInheritanceInfo = &inheritanceInfo;

	// Pipelines
	//Model object pipeline
	bs::vk::createPipelineDefault(*p_device, gfx, *m_renderpass, playout, desclayout);

	jobSystem.wait();
}

void GeneralRenderer::addInstance(bs::GameObject& entity)
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
	PushConstantsStruct pushconst = {};
	
	pushconst.textureids = bs::vec4(
		2,	//first is texture
		1,	//second is normals
		0,	//third is ???
		0	//forth is ???
	);

	VkExtent2D extent = { };		//For Dynamic State
		extent.height = bs::vk::viewportheight;
		extent.width = bs::vk::viewportwidth;
	
	// AUTISTIC THING : THIS STARTS THE RECORDING FOR THE IMGUI SECONDARY
	// The idea is thet I reserve the 0th index of the secondary cmd buffers for imgui rendering
	if (vkBeginCommandBuffer(m_renderlist.at(0), &beginInfo) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to begin recording command buffer!");
	}
	
	for (uint8_t i = 1; i < m_queue.size() + 1; ++i)
	{
		if (vkBeginCommandBuffer(m_renderlist.at(i), &beginInfo) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}
		
		vkCmdBindPipeline(m_renderlist.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, gfx);
		
		VkViewport viewport = {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = extent.width;
		viewport.height = extent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = extent;

		vkCmdSetViewport(m_renderlist.at(i), 0, 1, &viewport);
		vkCmdSetScissor(m_renderlist.at(i), 0, 1, &scissor);

		vkCmdBindDescriptorSets(m_renderlist.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, playout, 0, 1, 
			bs::asset_manager->pDescsetglobal, 0, nullptr);
		

		pushconst.textureids = bs::vec4		//uncomment when it actually becomes necessary to render >1 object that have different textures
		(
			m_queue.at(i - 1).material.texture_id,	//first is texture
			m_queue.at(i - 1).material.normal_id,	//second is normals
			0,	//third is ???
			0	//forth is ???
		);
		

		vkCmdPushConstants(m_renderlist.at(i), playout,
			VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PushConstantsStruct), &pushconst);


		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(m_renderlist.at(i), 0, 1, &bs::asset_manager->getModel(std::move(m_queue.at(i - 1).model_id)).getVertexBuffer()->getAPIResource(), &offset);
		
		vkCmdBindIndexBuffer(m_renderlist.at(i), bs::asset_manager->getModel(std::move(m_queue.at(i - 1).model_id)).getIndexBuffer()->getAPIResource(), offset, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(m_renderlist.at(i), bs::asset_manager->getModel(std::move(m_queue.at(i - 1).model_id)).getIndexBuffer()->getNumElements(), 1, 0, 0, 0);

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

// Get list of secondary cmd buffers
std::vector<VkCommandBuffer>& GeneralRenderer::getRenderlists()
{
	return m_renderlist;
}

GeneralRenderer::~GeneralRenderer()
{
	vkDestroyPipelineLayout(p_device->getDevice(), playout, nullptr);
	vkDestroyPipeline(p_device->getDevice(), gfx, nullptr);
	vkDestroyCommandPool(p_device->getDevice(), m_pool, nullptr);
}
