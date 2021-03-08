#include "GeneralRenderer.h"


GeneralRenderer::GeneralRenderer(vn::Device* mainDevice, VkRenderPass* rpass) : m_descriptorbuffer(vn::vk::BufferDescription{ {}, *mainDevice })
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
	
	inheritanceInfo.framebuffer = VK_NULL_HANDLE;

	beginInfo.pInheritanceInfo = &inheritanceInfo;

	// Mesh
	vn::vk::BufferDescription bufferdesc = {};
	bufferdesc.dev = *p_device;

	vn::Mesh mesh;
	vn::Vertex vert;

	vert.position = vn::vec3(-0.5f, 0.5f, 0.6f);
	mesh.vertices.push_back(vert);

	vert.position = vn::vec3(0.5f, 0.5f, 0.6f);
	mesh.vertices.push_back(vert);

	vert.position = vn::vec3(0.0f, -0.5f, 0.6f);
	mesh.vertices.push_back(vert);

	mesh.indicies.push_back(0);
	mesh.indicies.push_back(1);
	mesh.indicies.push_back(2);
	//bufferdesc.m_mesh = mesh;
	bufferdesc.m_mesh = vn::loadMeshFromObj("res/Models/sphere.obj");
	vn::vk::Buffer buffer(bufferdesc);
	buffer.uploadMesh();
	m_meshbuffers.emplace_back(buffer);

	// Descriptor Sets
	/*VkDescriptorPoolCreateInfo descpoolinfo{};
	
	descpoolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descpoolinfo.pNext = nullptr;
	descpoolinfo.poolSizeCount = 1;
	VkDescriptorPoolSize descpoolsize{};
	descpoolsize.descriptorCount = 1;
	descpoolsize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descpoolinfo.pPoolSizes = &descpoolsize;
	descpoolinfo.maxSets = 1;

	vkCreateDescriptorPool(p_device->getDevice(), &descpoolinfo, nullptr, &m_descpool);

	VkDescriptorSetLayoutBinding setlayoutbinding{};
	setlayoutbinding.binding = 0;
	setlayoutbinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	setlayoutbinding.stageFlags = VK_SHADER_STAGE_ALL;

	VkDescriptorSetLayoutCreateInfo desclayoutinfo{};
	desclayoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	desclayoutinfo.pNext = nullptr;
	desclayoutinfo.bindingCount = 0;
	desclayoutinfo.pBindings = ;

	VkDescriptorSetLayout desclayout;
	/// TODO: FINISH DESCRIPTOR SETS (DO IT BINDLESS)
	vkCreateDescriptorSetLayout(p_device->getDevice(), , nullptr, &desclayout);*/

	// Pipelines
	//vn::vk::createGraphicsPipeline(*m_renderpass, playout, gfx, mainDevice->getDevice());
	vn::vk::createPipeline(*p_device, gfx, *m_renderpass, playout);
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
		vkCmdBindVertexBuffers(m_renderlist.at(i), 0, 1, &m_meshbuffers[0].getAPIResource(), &offset);
		
		vkCmdBindIndexBuffer(m_renderlist.at(i), m_meshbuffers[0].m_index, offset, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(m_renderlist.at(i), m_meshbuffers[0].getNumElements(), 1, 0, 0, 0);

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

std::vector<VkCommandBuffer>& GeneralRenderer::getRenderlists()
{
	return m_renderlist;
}

GeneralRenderer::~GeneralRenderer()
{

}
