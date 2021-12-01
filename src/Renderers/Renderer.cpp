#include "Renderer.h"

#include <imgui.h>
#include <algorithm>


constexpr int numDescriptors = 2;

Renderer::Renderer(bs::Device* renderingDevice)
{
	device = renderingDevice;

	//Add textures
	//Create image + texture
	bs::vk::Texture font(device);
	bs::asset_manager->addTexture(font, 0);	//adds empty texture, gets updated later

	//Texture
	/*bs::Image img2;
	if (img2.loadFromFile("res/papertexture.jpg"))
	{
		std::cout << "Image creation success \n";
	}
*//*
	bs::vk::Texture texture(renderingDevice);
	texture.loadFromImage(img2);
	bs::asset_manager->addTexture(texture, 1);*/

	
	//Blank white img
	bs::Image imgblank;
	imgblank.create(32, 32, bs::u8vec4(255));

// Duping img
	bs::vk::Texture texture(renderingDevice);
	texture.loadFromImage(imgblank);
	bs::asset_manager->addTexture(texture, 1);
//Img

	//Generate a special image: (diagonal purple)
	{
		constexpr bs::u8vec4 purple = bs::u8vec4(255, 0, 255, 255);
		for(int y = 0; y < imgblank.getSize().y; ++y)
		{
			for(int x = 0; x < (imgblank.getSize().x - y); ++x)
			{
				imgblank.setPixel(x, y, purple);
			}
		}
	}
	bs::vk::Texture textureblank(renderingDevice);
	textureblank.loadFromImage(imgblank);
	bs::asset_manager->addTexture(textureblank, 2);

	//For IMGUI
	initGUI();

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

		if (vkCreateRenderPass(device->getDevice(), &renderPassInfo, nullptr, &renderpassdefault) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create render pass!");
		}
	}
	bs::vk::createCommandPool(*device, m_pool);

	//m_primaryBuffers.resize(bs::vk::NUM_SWAPCHAIN_FRAMEBUFFERS);
	m_primaryBuffers.resize(1);

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
	//std::cout << "Num Textures: " << bs::asset_manager->getNumTextures() << "\n";

	VkDescriptorPoolCreateInfo descpoolinfo{};	//Collapse descriptors
	{
		descpoolinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descpoolinfo.pNext = nullptr;
		descpoolinfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;

		VkDescriptorPoolSize descpoolsize[numDescriptors] = {};
		descpoolsize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descpoolsize[0].descriptorCount = 1;

		descpoolsize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		descpoolsize[1].descriptorCount = bs::asset_manager->getNumTextures();

		descpoolinfo.pPoolSizes = &descpoolsize[0];
		descpoolinfo.poolSizeCount = numDescriptors;

		descpoolinfo.maxSets = 15;

		VkResult result = vkCreateDescriptorPool(device->getDevice(), &descpoolinfo, nullptr, &m_descpool);
		
		// Descriptor Sets
		VkDescriptorSetLayoutBinding setlayoutbinding[numDescriptors] = {};
		setlayoutbinding[0].binding = 0;
		setlayoutbinding[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		setlayoutbinding[0].stageFlags = VK_SHADER_STAGE_ALL;
		setlayoutbinding[0].descriptorCount = 1;

		setlayoutbinding[1].binding = 1;
		setlayoutbinding[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		setlayoutbinding[1].stageFlags = VK_SHADER_STAGE_ALL;
		setlayoutbinding[1].descriptorCount = bs::asset_manager->getNumTextures();


		//For texture indexing:
		VkDescriptorSetLayoutBindingFlagsCreateInfo layoutbindingflags = {};
		layoutbindingflags.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO;
		layoutbindingflags.bindingCount = numDescriptors;
		VkDescriptorBindingFlags flags[numDescriptors] = { 0, VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT };
		layoutbindingflags.pBindingFlags = flags;
		

		VkDescriptorSetLayoutCreateInfo desclayoutinfo{};
		desclayoutinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		desclayoutinfo.pNext = &layoutbindingflags;
		desclayoutinfo.bindingCount = numDescriptors;
		desclayoutinfo.pBindings = &setlayoutbinding[0];
		desclayoutinfo.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT;
		
		result = vkCreateDescriptorSetLayout(device->getDevice(), &desclayoutinfo, nullptr, &desclayout);

		VkDescriptorSetAllocateInfo descriptorAllocInfo{};
		descriptorAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

		//For descriptor indexing
		VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescAlloc = {};
		variableDescAlloc.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO;
		variableDescAlloc.descriptorSetCount = 1;
		uint32_t varDescCount[] = { (uint32_t)bs::asset_manager->getNumTextures() };
		variableDescAlloc.pDescriptorCounts = varDescCount;

		descriptorAllocInfo.pNext = &variableDescAlloc;
		descriptorAllocInfo.descriptorPool = m_descpool;
		descriptorAllocInfo.descriptorSetCount = 1;
		descriptorAllocInfo.pSetLayouts = &desclayout;
		
		result = vkAllocateDescriptorSets(device->getDevice(), &descriptorAllocInfo, &m_descsetglobal);
		if(result != VK_SUCCESS)
		{
			std::cout << "Allocate Descriptor Sets Failed, result = " << result << "\n";
		}
	}
	bs::asset_manager->pDescsetglobal = &m_descsetglobal;

	struct MVP
	{
		bs::mat4 proj;
		bs::mat4 view;
		bs::mat4 model;
	} uniformbufferthing;

	// Descriptor Set Buffers:

	// Uniform buffer
	bs::vk::BufferDescription uniform;
	uniform.bufferType = bs::vk::BufferUsage::UNIFORM_BUFFER;
	uniform.dev = device;
	uniform.size = sizeof(MVP);
	uniform.stride = 64;
	uniform.bufferData = &uniformbufferthing;

	bs::asset_manager->addBuffer(new bs::vk::Buffer(uniform), "MVP");
	bs::asset_manager->getBuffer("MVP")->uploadBuffer();

	m_generalRenderer = std::make_unique<GeneralRenderer>(renderingDevice, &renderpassdefault, desclayout);

	bs::vk::createUIPipeline(*device, imguipipeline, renderpassdefault, guilayout, desclayout);

	m_UIRenderer = std::make_unique<UIRenderer>(renderingDevice, imguipipeline, guilayout);
}

void Renderer::initGUI()
{
	//Init IMGUI
	ImGuiIO& io = ImGui::GetIO();
	//IMGUI STUFF
	{
		uint8_t zeroarray[120] = { 0 };

		bs::vk::BufferDescription vbufdesc = {};
		vbufdesc.bufferType = bs::vk::VERTEX_BUFFER;
		vbufdesc.bufferData = zeroarray;
		vbufdesc.dev = device;
		vbufdesc.stride = sizeof(ImDrawVert);
		vbufdesc.size = 120;

		
		bs::vk::BufferDescription ibufdesc = {};
		ibufdesc.bufferType = bs::vk::INDEX_BUFFER;
		ibufdesc.bufferData = zeroarray;
		ibufdesc.dev = device;
		ibufdesc.stride = sizeof(ImDrawIdx);
		ibufdesc.size = 120;

		auto* buffer = new bs::vk::Buffer(vbufdesc);
		buffer->uploadBuffer();
		bs::asset_manager->addBuffer(buffer, "GUIvert");

		buffer = new bs::vk::Buffer(ibufdesc);
		buffer->uploadBuffer();
		bs::asset_manager->addBuffer(buffer, "GUIindex");
	}

	//IMGUI FONT STUFF
	{
		bs::Image imgfont;
		unsigned char* fontData;
		bs::vec2i fontsize;
		io.Fonts->AddFontDefault();

		io.Fonts->GetTexDataAsRGBA32(&fontData, &fontsize.x, &fontsize.y);

		imgfont.create(fontsize.x, fontsize.y, (bs::u8vec4*)fontData);
		
		bs::vk::Texture& font = bs::asset_manager->getTextureMutable(0);
		font.loadFromImage(imgfont);
		bs::asset_manager->addImg(imgfont, "font");
	}
}

void Renderer::drawObject(bs::GameObject& entity)
{
	m_generalRenderer->addInstance(entity);
}

void Renderer::drawText()
{
	m_UIRenderer->addText("Example Text", {500, 500});
}

void Renderer::render(Camera& cam)
{
	m_UIRenderer->render();

	ImGui::Render();
	ImGui::EndFrame();

	pushGPUData(cam);
	m_UIRenderer->bakeImGui();

	//Main Pass
	jobSystem.wait();

	m_generalRenderer->render(cam);

	jobSystem.wait();
}

void Renderer::finish(bs::vk::FramebufferData& fbo, int index)
{
	vkResetCommandPool(device->getDevice(), m_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
	//Second Pass
	auto renderLists = m_generalRenderer->getRenderlists(); // The secondary command buffers

	// FOR IM GUI BULLSHIT 
	{
		auto& cmd = renderLists.at(0);
		
		m_UIRenderer->finish(cmd);
	}

	//Primary Buffer Recording
	auto& cmd = m_primaryBuffers.at(0);
	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	if (vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) {
		throw std::runtime_error("failed to begin recording command buffer!");
	}

	VkRenderPassBeginInfo renderPassInfo{};

	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = renderpassdefault;
	renderPassInfo.framebuffer = fbo.handle[index];

	renderPassInfo.renderArea.offset = { 0, 0 };
	
	VkExtent2D extent;
	extent.height = bs::vk::viewportheight;
	extent.width = bs::vk::viewportwidth;

	renderPassInfo.renderArea.extent = extent;

	//VkClearDepthStencilValue depthClear = {};
	VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	renderPassInfo.clearValueCount = 1;
	renderPassInfo.pClearValues = &clearColor;

	//VK_SUBPASS_CONTENTS_INLINE //VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
	vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

	//Execute all the cmd buffers for the general renderer
	vkCmdExecuteCommands(cmd, renderLists.size() - 1, &renderLists[1]);
	//AFTER ^ is done, THEN submit the ImGui Draw
	vkCmdExecuteCommands(cmd, 1, &renderLists[0]);

	vkCmdEndRenderPass(cmd);

	if (vkEndCommandBuffer(cmd) != VK_SUCCESS) 
	{
		throw std::runtime_error("failed to record command buffer!");
	}
	

	//Submit main rendering
	device->submitWork(m_primaryBuffers);
}

void Renderer::clearQueue()
{
	m_generalRenderer->clearQueue();
}

void Renderer::pushGPUData(Camera& cam)
{
	//Buffer Writing Info
	VkDescriptorBufferInfo bufferInfo1 = {};
	auto* buf = bs::asset_manager->getBuffer("MVP");
	bufferInfo1.buffer = buf->getAPIResource();
	bufferInfo1.offset = 0;
	bufferInfo1.range = 192;

	bs::Transform t;
	t.pos.x = 0.0f;
	t.pos.y = 0.0f;
	t.pos.z = 0.0f;
	t.rot = bs::vec3(0.0f);
	//t.rescale(t, bs::vec3(0.0078125f));
	//t.rescale(t, bs::vec3(5616.0f, 1.0f, 2160.0f));
	//-0.500000 -0.500000 0.500000
	struct MVPstruct
	{
		bs::mat4 proj;
		bs::mat4 view;
		bs::mat4 model;
	};
	MVPstruct MVP = { 
		.proj	= cam.getProjMatrix(), 
		.view	= cam.getViewMatrix(), 
		.model	= bs::makeModelMatrix(t),
	};

	buf->writeBuffer(&MVP);

	//Image Writing Info
	const auto& textures = bs::asset_manager->getTextures();
	std::vector<VkDescriptorImageInfo> imageinfo;
	imageinfo.reserve(textures.size());

	for(int i = 0; i < textures.size(); ++i)
	{
		VkDescriptorImageInfo imginfo;
		imginfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		imginfo.imageView = textures[i].imgviewvk;
		imginfo.sampler = textures[i].sampler;

		imageinfo.emplace_back(imginfo);
	}

	
	
	//Writing Info
	VkWriteDescriptorSet descWrite[numDescriptors] = {};
	descWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descWrite[0].dstSet = m_descsetglobal;
	descWrite[0].dstBinding = 0;
	descWrite[0].dstArrayElement = 0; //Starting array element
	descWrite[0].descriptorCount = 1; //Number to write over
	descWrite[0].pBufferInfo = &bufferInfo1;

	descWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descWrite[1].dstSet = m_descsetglobal;
	descWrite[1].dstBinding = 1;
	descWrite[1].dstArrayElement = 0;
	descWrite[1].descriptorCount = imageinfo.size();
	descWrite[1].pImageInfo = imageinfo.data();

	vkUpdateDescriptorSets(device->getDevice(), numDescriptors, &descWrite[0], 0, nullptr);
	
	/*if(bs::asset_manager->loaded)
	{
		// VkDescriptorImageInfo cache;
		// cache.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		// cache.imageView = bs::asset_manager->pMapCache->stuff.imgviewvk;
		// cache.sampler = bs::asset_manager->pMapCache->stuff.sampler;

		// descWrite[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		// descWrite[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		// descWrite[3].dstSet = m_descsetglobal;
		// descWrite[3].dstBinding = 2;
		// descWrite[3].dstArrayElement = 0;
		// descWrite[3].descriptorCount = 1;
		// descWrite[3].pImageInfo = &cache;

		//Write Values to GPU
		vkUpdateDescriptorSets(device->getDevice(), numDescriptors, &descWrite[0], 0, nullptr);
	}
	else
	{
		//Write Values to GPU
		vkUpdateDescriptorSets(device->getDevice(), numDescriptors - 1, &descWrite[0], 0, nullptr);
	}*/
}

Renderer::~Renderer()
{
	// gui related
	vkDestroyPipeline(device->getDevice(), imguipipeline, nullptr);
	vkDestroyPipelineLayout(device->getDevice(), guilayout, nullptr);
	//
	vkDestroyRenderPass(device->getDevice(), renderpassdefault, nullptr);
	vkDestroyCommandPool(device->getDevice(), m_pool, nullptr);
	vkDestroyDescriptorSetLayout(device->getDevice(), desclayout, nullptr);
	vkDestroyDescriptorPool(device->getDevice(), m_descpool, nullptr);
}