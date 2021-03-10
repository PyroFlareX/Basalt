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
	descpoolinfo.poolSizeCount = 1; // 2;

	VkDescriptorPoolSize descpoolsize[2] = {};
	descpoolsize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descpoolsize[0].descriptorCount = 1;
	
	descpoolsize[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	descpoolsize[1].descriptorCount = 1;

	descpoolinfo.pPoolSizes = &descpoolsize[0];
	descpoolinfo.maxSets = 1;

	vkCreateDescriptorPool(p_device->getDevice(), &descpoolinfo, nullptr, &m_descpool);

	// Descriptor Sets
	VkDescriptorSetLayoutBinding setlayoutbinding[2] = {};
	setlayoutbinding[0].binding = 0;
	setlayoutbinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	setlayoutbinding[0].stageFlags = VK_SHADER_STAGE_ALL;
	setlayoutbinding[0].descriptorCount = 1;

	setlayoutbinding[1].binding = 1;
	setlayoutbinding[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	setlayoutbinding[1].stageFlags = VK_SHADER_STAGE_ALL;
	setlayoutbinding[1].descriptorCount = 1;

	VkDescriptorSetLayoutCreateInfo desclayoutinfo{};
	desclayoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	desclayoutinfo.pNext = nullptr;
	desclayoutinfo.bindingCount = 1; // 2;
	desclayoutinfo.pBindings = &setlayoutbinding[0];
	desclayoutinfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT_EXT;

	VkDescriptorSetLayout desclayout;
	
	vkCreateDescriptorSetLayout(p_device->getDevice(), &desclayoutinfo, nullptr, &desclayout);

	VkDescriptorSetAllocateInfo descriptorAllocInfo{};
	descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	descriptorAllocInfo.descriptorPool = m_descpool;
	descriptorAllocInfo.descriptorSetCount = 1;
	descriptorAllocInfo.pSetLayouts = &desclayout;

	vkAllocateDescriptorSets(p_device->getDevice(), &descriptorAllocInfo, &m_descsetglobal);
	
	/// TODO: FINISH DESCRIPTOR SETS (DO IT BINDLESS)

	struct test
	{
		float x = 1;
		float y = 2;
		float z = 3;
		float w = 4;
	};

	test* uniformbufferthing = new test;

	// Descriptor Set Buffers:
	vn::vk::BufferDescription uniform;
	uniform.bufferType = vn::BufferUsage::UNIFORM_BUFFER;
	uniform.dev = p_device;
	uniform.size = 4;
	uniform.stride = 4;
	uniform.bufferData = uniformbufferthing;

	vn::vk::BufferDescription storage;
	storage.bufferType = vn::BufferUsage::STORAGE_BUFFER;
	storage.dev = p_device;
	storage.size = 2;
	storage.stride = 64;

	m_descriptorBuffers.emplace_back(new vn::vk::Buffer(uniform));
	m_descriptorBuffers.emplace_back(new vn::vk::Buffer(storage));
	
	m_descriptorBuffers.at(0)->uploadBuffer();
	//m_descriptorBuffers.at(1)->uploadBuffer();

	VkDescriptorBufferInfo bufferInfo1{};
	bufferInfo1.buffer = m_descriptorBuffers.at(0)->getAPIResource();
	bufferInfo1.offset = 0;
	bufferInfo1.range = 16;

	VkDescriptorBufferInfo bufferInfo2{};
	bufferInfo2.buffer = m_descriptorBuffers.at(1)->getAPIResource();
	bufferInfo2.offset = 0;
	bufferInfo2.range = 128;

	VkWriteDescriptorSet descWrite{};
	descWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrite.dstSet = m_descsetglobal;
	descWrite.dstBinding = 0;
	descWrite.dstArrayElement = 0; // Double check later
	descWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descWrite.descriptorCount = 1;
	descWrite.pBufferInfo = &bufferInfo1;
	// Double check other values later

	vkUpdateDescriptorSets(p_device->getDevice(), 1, &descWrite, 0, nullptr);

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

		vkCmdBindDescriptorSets(m_renderlist.at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, playout, 0, 1, &m_descsetglobal, 0, nullptr);

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
