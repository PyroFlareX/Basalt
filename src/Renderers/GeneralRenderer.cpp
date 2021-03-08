#include "GeneralRenderer.h"


GeneralRenderer::GeneralRenderer(vn::Device* mainDevice, VkRenderPass* rpass) //: m_descriptorbuffer(vn::vk::BufferDescription{})
{
	p_device = mainDevice;
	vn::vk::createCommandPool(*p_device, m_pool);
	m_renderpass = rpass;

	img.loadFromFile("res/container.jpg");
	

	m_renderlist.resize(1);

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
	
	// Set to Null because the finish render function has the target frame buffer to render to
	inheritanceInfo.framebuffer = VK_NULL_HANDLE;

	beginInfo.pInheritanceInfo = &inheritanceInfo;

	// Mesh
	m_models.emplace_back(new vn::vk::Model(vn::loadMeshFromObj("res/Models/sphere.obj"), p_device));

	

	// Descriptor Pools
	VkDescriptorPoolCreateInfo descpoolinfo{};
	
	descpoolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descpoolinfo.pNext = nullptr;
	descpoolinfo.poolSizeCount = 1;

	VkDescriptorPoolSize descpoolsize{};
	descpoolsize.descriptorCount = 1;
	descpoolsize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;

	descpoolinfo.pPoolSizes = &descpoolsize;
	descpoolinfo.maxSets = 1;

	vkCreateDescriptorPool(p_device->getDevice(), &descpoolinfo, nullptr, &m_descpool);

	// Descriptor Sets
	VkDescriptorSetLayoutBinding setlayoutbinding{};
	setlayoutbinding.binding = 0;
	setlayoutbinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	setlayoutbinding.stageFlags = VK_SHADER_STAGE_ALL;
	setlayoutbinding.descriptorCount = 1;

	VkDescriptorSetLayoutCreateInfo desclayoutinfo{};
	desclayoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	desclayoutinfo.pNext = nullptr;
	desclayoutinfo.bindingCount = 1;
	desclayoutinfo.pBindings = &setlayoutbinding;

	VkDescriptorSetLayout desclayout;
	/// TODO: FINISH DESCRIPTOR SETS (DO IT BINDLESS)
	vkCreateDescriptorSetLayout(p_device->getDevice(), &desclayoutinfo, nullptr, &desclayout);

	// Pipelines
	vn::vk::createPipeline(*p_device, gfx, *m_renderpass, playout, desclayout);
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

	PushConstantsStruct pushconst = {};
	pushconst.proj = cam.getProjMatrix();
	pushconst.view = cam.getViewMatrix();
	vn::Transform t;
	t.pos.x = 0.0f;
	t.pos.y = 0.0f;
	t.pos.z = 0.0f;

	t.rescale(t, vn::vec3(0.5f, 0.5f, 0.5f));

	pushconst.model = vn::makeModelMatrix(t);
	

	for (uint8_t i = 0; i < m_queue.size(); ++i)
	{
		if (vkBeginCommandBuffer(m_renderlist.at(i), &beginInfo) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		vkCmdBindPipeline(m_renderlist.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, gfx);

		vkCmdPushConstants(m_renderlist.at(i), playout,
			VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantsStruct), &pushconst);

		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(m_renderlist.at(i), 0, 1, &m_models.at(0)->getVertexBuffer()->getAPIResource(), &offset);
		
		vkCmdBindIndexBuffer(m_renderlist.at(i), m_models.at(0)->getIndexBuffer()->getAPIResource(), offset, VK_INDEX_TYPE_UINT32);
		
		vkCmdDrawIndexed(m_renderlist.at(i), m_models.at(0)->getIndexBuffer()->getNumElements(), 1, 0, 0, 0);

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
	for (auto* model : m_models)
	{
		delete model;
	}
}
