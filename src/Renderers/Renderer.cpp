#include "Renderer.h"

#include <imgui.h>
#include <algorithm>

#include "../World/Meshing/MeshingData.h"

constexpr int numDescriptors = 8;

Renderer::Renderer(bs::Device* renderingDevice, VkRenderPass genericPass)	: device(renderingDevice), m_renderpassdefault(genericPass)
{
	//Add textures
	//Create image + texture
	bs::vk::Texture font(device);
	//Adds empty texture, gets updated later, index 0 is the font texture
	bs::asset_manager->addTexture(font, 0);	
	
	//Blank white img
	const bs::Image imgblank({32, 32}, bs::u8vec4(255));

	// Duping img
	bs::vk::Texture texture(device);
	texture.loadFromImage(imgblank);
	//Adding black white image for texture index 1
	bs::asset_manager->addTexture(texture, 1);

	//Generate a special image: (diagonal purple)
	constexpr bs::u8vec4 purple = bs::u8vec4(255, 0, 255, 255);
	constexpr bs::u8vec4 cyan = bs::u8vec4(64, 225, 205, 255);
	bs::Image notFoundImg(imgblank.getSize(), cyan);
	{
		//This assumes a square img
		const auto numColumns = notFoundImg.getSize().y;
		for(auto y = 0; y < numColumns; y += 1)
		{
			//Calc the number of pixels to set on this row
			auto lengthToSet = numColumns - y;

			for(auto x = 0; x < lengthToSet; x+=1)
			{
				notFoundImg.setPixel(x, y, purple);
			}
		}
	}
	bs::vk::Texture textureblank(device);
	textureblank.loadFromImage(notFoundImg);
	//Adding a diagonal purple texture for index 2
	bs::asset_manager->addTexture(textureblank, 2);

	//For IMGUI
	initGUI();
	
	//Init the command pool and the cmd buffer
	initCommandPoolAndBuffers();

	//Create the descriptor sets info
	std::vector<DescriptorSetInfo> descriptorInfos;
	u32 slots = 0;

	descriptorInfos.emplace_back(DescriptorSetInfo
	{
		.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		.bindingSlot = slots++,
		.count = 1,
	});

	descriptorInfos.emplace_back(DescriptorSetInfo
	{
		.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		.bindingSlot = numDescriptors - 1,
		.count = (u32)bs::asset_manager->getNumTextures(),
	});

	descriptorInfos.emplace_back(DescriptorSetInfo
	{
		.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		.bindingSlot = slots++,
		.count = 1,
	});

	// Descriptor Pool
	initDescriptorPool(descriptorInfos);
	// Descriptor Set
	initDescriptorSets(descriptorInfos);

	// Initializing the descriptor set buffers
	initDescriptorSetBuffers(descriptorInfos);

	//Create General Renderer
	m_generalRenderer = std::make_unique<GeneralRenderer>(device, m_renderpassdefault, desclayout);
	//Create the chunk renderer
	m_chunkRenderer = std::make_unique<ChunkRenderer>(device, m_renderpassdefault, desclayout);
	//Create the UI Renderer
	m_UIRenderer = std::make_unique<UIRenderer>(device, m_renderpassdefault, desclayout);
}

Renderer::~Renderer()
{
	// Destroy the rest of the Vulkan allocations
	vkDestroyCommandPool(device->getDevice(), m_pool, nullptr);
	vkDestroyDescriptorSetLayout(device->getDevice(), desclayout, nullptr);
	vkDestroyDescriptorPool(device->getDevice(), m_descpool, nullptr);
}

void Renderer::initGUI()
{
	//Init IMGUI
	ImGuiIO& io = ImGui::GetIO();
	//IMGUI STUFF
	{
		bs::vk::BufferDescription vbufdesc = {};
		vbufdesc.bufferType = bs::vk::VERTEX_BUFFER;
		vbufdesc.dev = device;
		vbufdesc.stride = sizeof(ImDrawVert);
		vbufdesc.size = 500;
		
		bs::vk::BufferDescription ibufdesc = {};
		ibufdesc.bufferType = bs::vk::INDEX_BUFFER;
		ibufdesc.dev = device;
		ibufdesc.stride = sizeof(ImDrawIdx);
		ibufdesc.size = 500;

		////Create the ImGui Vertex Buffer
		bs::asset_manager->addBuffer(std::make_shared<bs::vk::Buffer>(vbufdesc), "GUIvert");

		//Create the ImGui Index Buffer
		bs::asset_manager->addBuffer(std::make_shared<bs::vk::Buffer>(ibufdesc), "GUIindex");
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

void Renderer::drawObject(const bs::GameObject& entity)
{
	m_generalRenderer->addInstance(entity);
}

void Renderer::drawText()
{
	m_UIRenderer->addText("Example Text", {500, 500});
}

void Renderer::recreateChunkDrawLists()
{
	m_chunkRenderer->clearCommandBuffer();
}

void Renderer::passChunkMeshGenerator(const void* chunk_mesh_manager)
{
	m_chunkRenderer->p_mesh_manager = (const ChunkMeshManager*)chunk_mesh_manager;
}

void Renderer::render(Camera& cam)
{
	// m_UIRenderer->ImGuiRender();

	ImGui::Render();
	ImGui::EndFrame();

	pushGPUData(cam);
	m_UIRenderer->bakeImGui();

	//Main Pass
	// auto& jobSystem = bs::getJobSystem();
	// jobSystem.wait(0);

	m_generalRenderer->render(cam);
	m_UIRenderer->render();
	m_chunkRenderer->buildRenderCommands();

	// jobSystem.wait();
}

void Renderer::finish(bs::vk::FramebufferData& fbo, int index)
{	
	//Clear values for renderpass
	const VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
	VkClearValue clearDepth = { };
	clearDepth.depthStencil.depth = 1.0f;
	const VkClearValue clearValues[2] = { clearColor, clearDepth };

	//Extent defining for renderpass
	const VkExtent2D extent
	{
		.width = bs::vk::viewportwidth,
		.height = bs::vk::viewportheight,
	};

	//Renderpass info stuff
	const VkRenderPassBeginInfo renderPassInfo
	{
		.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
		.pNext = nullptr,
		.renderPass = m_renderpassdefault,
		.framebuffer = fbo.handle[index],
		.renderArea = 
		{
			.offset = { 0, 0 },
			.extent = extent,
		},

		.clearValueCount = 2,
		.pClearValues = &clearValues[0],
	};	

	//Primary Buffer Recording
	//Begin Info
	constexpr VkCommandBufferBeginInfo beginInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = 0,
		.pInheritanceInfo = nullptr,
	};

	for(auto& cmd : m_primaryBuffers)
	{
		//Begin Recording the buffer
		if(vkBeginCommandBuffer(cmd, &beginInfo) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to begin recording command buffer!");
		}
		//Begin the renderpass
		//VK_SUBPASS_CONTENTS_INLINE //VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
		vkCmdBeginRenderPass(cmd, &renderPassInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

		///	ACTUAL RECORDING DONE HERE:

		/**
		 * Order: (if/when it matters, depth buffer makes this partially irrelevant)
		 * #1: General Renderer
		 * ...
		 * Chunk Renderer
		 * ...
		 * The others
		 * ...
		 * #end: UI Renderer
		**/
		
		//Execute all the cmd buffers for the general renderer
		m_generalRenderer->executeCommands(cmd);

		//Chunk Renderer
		m_chunkRenderer->executeCommands(cmd);
		//OTHERS
		// ...

		//UI Renderer
		m_UIRenderer->executeCommands(cmd);

		/// ENDING THE RECORDING
		vkCmdEndRenderPass(cmd);
		if(vkEndCommandBuffer(cmd) != VK_SUCCESS) 
		{
			throw std::runtime_error("Failed to record command buffer!");
		}
	}

	//Submit main rendering
	device->submitWork(m_primaryBuffers);
}

void Renderer::clearQueue()
{
	m_generalRenderer->clearQueue();
	m_chunkRenderer->clearCommandBuffer();
	vkResetCommandPool(device->getDevice(), m_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
}

void Renderer::pushGPUData(Camera& cam)
{
	//Buffer Writing Info
	auto mvp_buf = bs::asset_manager->getBuffer("MVP");

	const bs::Transform t;
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
	//Update the camera buffer
	mvp_buf->writeBuffer(&MVP);

	//Image Writing Info
	//Collect textures
	const auto& textures = bs::asset_manager->getTextures();
	std::vector<VkDescriptorImageInfo> imageinfo;
	imageinfo.reserve(textures.size());

	for(auto i = 0; i < textures.size(); ++i)
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
	descWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descWrite[0].dstSet = m_descsetglobal;
	descWrite[0].dstBinding = numDescriptors - 1;
	descWrite[0].dstArrayElement = 0;
	descWrite[0].descriptorCount = imageinfo.size();
	descWrite[0].pImageInfo = imageinfo.data();

	//Update the descriptor to the current texture handles
	vkUpdateDescriptorSets(device->getDevice(), 1, &descWrite[0], 0, nullptr);
}

void Renderer::initCommandPoolAndBuffers()
{
	bs::vk::createCommandPool(*device, m_pool);

	m_primaryBuffers.resize(1);

	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_pool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = m_primaryBuffers.size();
	
	VkResult result = vkAllocateCommandBuffers(device->getDevice(), &allocInfo, m_primaryBuffers.data());
	if(result != VK_SUCCESS) 
	{
		std::cerr << "Failed to allocate command buffers, error code: " << result << "\n";
		throw std::runtime_error("Failed to allocate command buffers!");
	}
}

void Renderer::initDescriptorPool(const std::vector<DescriptorSetInfo>& sets)
{
	//Some layouts stuff
	VkDescriptorPoolSize desc_pool_size[numDescriptors];
	//Blank init the pool array
	for(auto i = 0; i < numDescriptors; i += 1)
	{
		desc_pool_size[i].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		desc_pool_size[i].descriptorCount = 1;
	}

	//Add the proper stuff to the pool
	for(const auto& descriptor : sets)
	{
		const u32 bindingSlot = descriptor.bindingSlot;
		desc_pool_size[bindingSlot].type = descriptor.type;
		desc_pool_size[bindingSlot].descriptorCount = descriptor.count;
	}

	//Descriptor pool stuff
	const VkDescriptorPoolCreateInfo desc_pool_info
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.pNext = nullptr,
		.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
		.maxSets = 100,

		.poolSizeCount = numDescriptors,
		.pPoolSizes = &desc_pool_size[0],
	};

	VkResult result = vkCreateDescriptorPool(device->getDevice(), &desc_pool_info, nullptr, &m_descpool);
	if(result != VK_SUCCESS)
	{
		std::cerr << "Creating Descriptor Pool Failed, result = " << result << "\n";
		throw std::runtime_error("Creating Descriptor Pool Failed!!!");
	}
}

void Renderer::initDescriptorSets(const std::vector<DescriptorSetInfo>& sets)
{
	// Descriptor Sets
	VkDescriptorSetLayoutBinding setLayoutBinding[numDescriptors];
	//Blank init the array
	for(auto i = 0; i < numDescriptors; i += 1)
	{
		setLayoutBinding[i].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		setLayoutBinding[i].binding = i;
		setLayoutBinding[i].descriptorCount = 1;
		setLayoutBinding[i].stageFlags = VK_SHADER_STAGE_ALL;
		setLayoutBinding[i].pImmutableSamplers = nullptr;
	}
	//Add the sets into it
	for(const auto& descriptor : sets)
	{
		const u32 bindingSlot = descriptor.bindingSlot;
		setLayoutBinding[bindingSlot].descriptorType = descriptor.type;
		setLayoutBinding[bindingSlot].binding = bindingSlot;
		setLayoutBinding[bindingSlot].descriptorCount = descriptor.count;
		setLayoutBinding[bindingSlot].stageFlags = VK_SHADER_STAGE_ALL;
		setLayoutBinding[bindingSlot].pImmutableSamplers = nullptr;
	}

	//For texture indexing:
	std::vector<VkDescriptorBindingFlags> flags(numDescriptors, 0);
	flags.at(7) = VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT;
	
	const VkDescriptorSetLayoutBindingFlagsCreateInfo layoutBindingFlags
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
		.pNext = nullptr,
		.bindingCount = numDescriptors,
		.pBindingFlags = flags.data(),
	};

	//Layout Creation for bindings
	const VkDescriptorSetLayoutCreateInfo descLayoutInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.pNext = nullptr, //&layoutBindingFlags,
		.flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
		.bindingCount = numDescriptors,
		.pBindings = &setLayoutBinding[0],
	};
		
	//Creating the layouts for the descriptor sets
	VkResult result = vkCreateDescriptorSetLayout(device->getDevice(), &descLayoutInfo, nullptr, &desclayout);
	if(result != VK_SUCCESS)
	{
		std::cerr << "Creating Descriptor Set Layout Failed, result = " << result << "\n";
	}

	//For descriptor indexing, pNext member
	const u32 varDescCount[] = { (u32)bs::asset_manager->getNumTextures() };
	const VkDescriptorSetVariableDescriptorCountAllocateInfo variableDescAlloc
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
		.pNext = nullptr,
		.descriptorSetCount = 1,
		.pDescriptorCounts = varDescCount,
	};

	//Descriptor Allocation Info
	const VkDescriptorSetAllocateInfo descriptorAllocInfo
	{
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.pNext = nullptr, //&variableDescAlloc,
		.descriptorPool = m_descpool,
		.descriptorSetCount = 1,
		.pSetLayouts = &desclayout,
	};
	
	//Vulkan call to allocate the descriptor sets
	result = vkAllocateDescriptorSets(device->getDevice(), &descriptorAllocInfo, &m_descsetglobal);
	if(result != VK_SUCCESS)
	{
		std::cerr << "Allocate Descriptor Sets Failed, result = " << result << "\n";
	}

	//Put the descriptor set as a global into the asset manager
	bs::asset_manager->pDescsetglobal = &m_descsetglobal;
}

void Renderer::initDescriptorSetBuffers(const std::vector<DescriptorSetInfo>& sets)
{
	typedef struct
	{
		bs::mat4 proj;
		bs::mat4 view;
	} ProjView;
	
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
	uniform.stride = sizeof(bs::mat4);
	uniform.bufferData = &uniformbufferthing;
	bs::asset_manager->addBuffer(std::make_shared<bs::vk::Buffer>(uniform), "MVP");
	auto mvp_buffer = bs::asset_manager->getBuffer("MVP");

	//CHUNK TEXTURE INDEX STORAGE BUFFERS
	//@TODO: THIS IS A TEST! Remove this when actually adding the textures

	constexpr auto storageType = bs::vk::BufferUsage::STORAGE_BUFFER;
	const auto NUM_CHUNKS = (2 * 2) * 4 * 16;
	bs::vk::BufferDescription basicDescription
	{
		.dev = device,
		.bufferType = storageType,
		.size = NUM_CHUNKS * NUM_FACES_IN_FULL_CHUNK * sizeof(u16),	// 48 KB per chunk
		.stride = sizeof(u16),
	};

	//Chunks Texture Indexing Storage Buffer
	bs::asset_manager->addBuffer(std::make_shared<bs::vk::Buffer>(basicDescription), "chunk_texture_data");
	auto face_texture_buffer = bs::asset_manager->getBuffer("chunk_texture_data");

	void* bufferptr = nullptr;
	vmaMapMemory(device->getAllocator(), face_texture_buffer->getAllocation(), &bufferptr);

	u16* buffer = (u16*)bufferptr;
	for(auto varCount = 0; varCount < face_texture_buffer->getSize() / sizeof(u16); varCount += 1)
	{
		buffer[varCount] = 1;
	}

	vmaUnmapMemory(device->getAllocator(), face_texture_buffer->getAllocation());


	//NOW Update the descriptor sets
	VkWriteDescriptorSet basicWrite;
	basicWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	basicWrite.pNext = nullptr;
	basicWrite.pBufferInfo = nullptr;
	basicWrite.pImageInfo = nullptr;
	basicWrite.pTexelBufferView = nullptr;
	basicWrite.dstSet = m_descsetglobal;
	basicWrite.dstArrayElement = 0; //Starting array element
	basicWrite.descriptorCount = 1; //Number to write over

	const VkDescriptorBufferInfo MVPBufferWrite
	{	//MVP
		.buffer = mvp_buffer->getAPIResource(),
		.offset = 0,
		.range = mvp_buffer->getSize(), // 192
	};

	const VkDescriptorBufferInfo textureBufferWrite
	{	//Chunk Texture Info
		.buffer = face_texture_buffer->getAPIResource(),
		.offset = 0,
		.range = face_texture_buffer->getSize(), // NUM_FACES_IN_FULL_CHUNK * sizeof(u16),
	};

	//To write the actual descriptor set
	VkWriteDescriptorSet writeSetsArray[numDescriptors] = {};

	writeSetsArray[0] = basicWrite;
	writeSetsArray[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeSetsArray[0].dstBinding = 0;
	writeSetsArray[0].descriptorCount = 1;
	writeSetsArray[0].pBufferInfo = &MVPBufferWrite;

	writeSetsArray[1] = basicWrite;
	writeSetsArray[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	writeSetsArray[1].dstBinding = 1;
	writeSetsArray[1].descriptorCount = 1;
	writeSetsArray[1].pBufferInfo = &textureBufferWrite;

	vkUpdateDescriptorSets(device->getDevice(), 2, &writeSetsArray[0], 0, nullptr);
}