#include "Renderer.h"

#include <imgui.h>
#include <algorithm>

constexpr auto numDescriptors = 8;

constexpr auto fontTextureID = 0;

Renderer::Renderer(bs::Device* renderingDevice, VkRenderPass genericPass)	: device(renderingDevice), m_renderpassdefault(genericPass)
{
	//Add textures
	//Create image + texture
	auto font = std::make_shared<bs::vk::Texture>(device);
	//Adds empty texture, gets updated later, index 0 is the font texture
	bs::asset_manager->addTexture(font, fontTextureID);	
	
	//Blank white img
	const bs::Image imgblank({32, 32}, bs::u8vec4(255));

	// Duping img
	auto texture = std::make_shared<bs::vk::Texture>(device, imgblank);
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
	
	auto textureblank = std::make_shared<bs::vk::Texture>(device, notFoundImg);
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

	// descriptorInfos.emplace_back(DescriptorSetInfo
	// {
	// 	.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
	// 	.bindingSlot = slots++,
	// 	.count = 1,
	// });

	// Descriptor Pool
	initDescriptorPool(descriptorInfos);
	// Descriptor Set
	initDescriptorSets(descriptorInfos);

	// Initializing the descriptor set buffers
	initDescriptorSetBuffers(descriptorInfos);

	//Create General Renderer
	m_generalRenderer = std::make_unique<GeneralRenderer>(device, m_renderpassdefault, desclayout);
	
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
	//Create the ImGui Vertex Buffer
	auto guiVert = std::make_shared<bs::vk::Buffer>(device, bs::vk::VERTEX_BUFFER, 500, sizeof(ImDrawVert));
	bs::asset_manager->addBuffer(guiVert, "GUIvert");

	//Create the ImGui Index Buffer
	auto guiIndex = std::make_shared<bs::vk::Buffer>(device, bs::vk::INDEX_BUFFER, 500, sizeof(ImDrawIdx));
	bs::asset_manager->addBuffer(guiIndex, "GUIindex");
	
	//IMGUI FONT STUFF
	ImGuiIO& io = ImGui::GetIO();

	u8* fontData;
	bs::vec2i fontsize;

	//Add font
	io.Fonts->AddFontDefault();
	io.Fonts->GetTexDataAsRGBA32(&fontData, &fontsize.x, &fontsize.y);

	//Font Image Data
	const bs::Image imgfont(fontsize, (bs::u8vec4*)fontData);
	//Font to the texture
	io.Fonts->SetTexID(bs::cast(fontTextureID));
	bs::asset_manager->getTextureMutable(fontTextureID).loadFromImage(imgfont);
}

void Renderer::drawObject(const bs::GameObject& entity)
{
	m_generalRenderer->addInstance(entity);
}

void Renderer::drawText()
{
	m_UIRenderer->addText("Example Text", {500, 500});
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
		 * ...
		 * The others
		 * ...
		 * #end: UI Renderer
		**/
		
		//Execute all the cmd buffers for the general renderer
		m_generalRenderer->executeCommands(cmd);

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
	static std::vector<VkDescriptorImageInfo> prev_infos;
	imageinfo.reserve(textures.size());

	for(auto i = 0; i < textures.size(); ++i)
	{
		imageinfo.emplace_back(VkDescriptorImageInfo{ 
			.sampler =  textures[i].sampler,
			.imageView = textures[i].imgviewvk,
			.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		});
	}
	
	/**
	 * @brief 
	 * Algorithm for updating ONLY the existing textures
	 * 
	 * Hold array element and count
	 * Skip the empty ones [they have VK_NULL_HANDLE]
	 * OR just use the "blank texture"
	 */

	std::vector<VkWriteDescriptorSet> textureWrites;
	textureWrites.reserve(imageinfo.size());
	for(auto i = 0; i < imageinfo.size(); ++i)
	{
		VkWriteDescriptorSet write;
		write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write.pNext = nullptr;
		write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write.dstSet = m_descsetglobal;
		write.dstBinding = numDescriptors - 1;
		write.descriptorCount = 1;

		//The element to write to
		write.dstArrayElement = i;

		//If the image for the index is not there, assign the "blank image" instead
		if(imageinfo[i].imageView != VK_NULL_HANDLE)
		{	
			//Ptr to the image info (array)
			write.pImageInfo = &imageinfo[i];
		}
		else
		{
			//An assertion to ensure that the default texure is valid and exists
			assert(imageinfo.at(1).imageView != VK_NULL_HANDLE);
			write.pImageInfo = &(imageinfo.at(1));
		}

		if(i < prev_infos.size())
		{
			if(prev_infos[i].imageView == imageinfo[i].imageView)
			{
				continue;
			}
		}

		textureWrites.emplace_back(write);
	}

	prev_infos = imageinfo;

	//Update the descriptor to the current texture handles
	vkUpdateDescriptorSets(device->getDevice(), textureWrites.size(), textureWrites.data(), 0, nullptr);
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
	/*bs::vk::BufferDescription uniform;
	uniform.bufferType = bs::vk::BufferUsage::UNIFORM_BUFFER;
	uniform.dev = device;
	uniform.size = sizeof(MVP);
	uniform.stride = sizeof(bs::mat4);
	uniform.bufferData = &uniformbufferthing;*/
	bs::asset_manager->addBuffer(std::make_shared<bs::vk::Buffer>(device, bs::vk::BufferUsage::UNIFORM_BUFFER, 
									sizeof(MVP), sizeof(bs::mat4), &uniformbufferthing), "MVP");
	auto mvp_buffer = bs::asset_manager->getBuffer("MVP");

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


	//To write the actual descriptor set
	VkWriteDescriptorSet writeSetsArray[numDescriptors] = {};

	writeSetsArray[0] = basicWrite;
	writeSetsArray[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeSetsArray[0].dstBinding = 0;
	writeSetsArray[0].descriptorCount = 1;
	writeSetsArray[0].pBufferInfo = &MVPBufferWrite;

	// writeSetsArray[1] = basicWrite;
	// writeSetsArray[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
	// writeSetsArray[1].dstBinding = 1;
	// writeSetsArray[1].descriptorCount = 1;
	// writeSetsArray[1].pBufferInfo = &textureBufferWrite;

	vkUpdateDescriptorSets(device->getDevice(), 1, &writeSetsArray[0], 0, nullptr);
}