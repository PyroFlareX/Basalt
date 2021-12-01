#include "PipelineBuilder.h"

#include "Device.h"
#include "../../Util/Loaders.h"

const static auto shaderName = "main";

namespace bs::vk
{
	GraphicsPipelineBuilder::GraphicsPipelineBuilder(bs::Device* mainDevice, VkRenderPass renderpass, VkDescriptorSetLayout& desclayout)	: 
			isBuilt(false), m_frag(false), m_vert(false), p_device(mainDevice), pushConstantStructSize(0), numBindings(0)
	{
		m_shaderModules.resize(2);

		//Basic Pipeline struct filling info
		m_gfxCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		m_gfxCreateInfo.pNext = nullptr;
		m_gfxCreateInfo.subpass = 0;
		m_gfxCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		m_gfxCreateInfo.flags = 0;
		
		m_gfxCreateInfo.renderPass = renderpass;
		//Basic pipeline layout struct filling info
		m_layoutinfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		m_layoutinfo.pNext = nullptr;
		m_layoutinfo.flags = 0;
		m_layoutinfo.setLayoutCount = 1;
		m_layoutinfo.pSetLayouts = &desclayout;
		m_layoutinfo.pushConstantRangeCount = 0;
		m_layoutinfo.pPushConstantRanges = nullptr;

		//Default rasterizer stuff
		m_rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		m_rasterizer.pNext = nullptr;
		m_rasterizer.flags = 0;
		m_rasterizer.depthClampEnable = VK_FALSE;
		m_rasterizer.rasterizerDiscardEnable = VK_FALSE;
		m_rasterizer.depthBiasEnable = VK_FALSE;
		m_rasterizer.lineWidth = 1.0f;
		//Default value
		m_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		m_rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		//Rasterizer State
		m_gfxCreateInfo.pRasterizationState = &m_rasterizer;
	}

	GraphicsPipelineBuilder::~GraphicsPipelineBuilder()
	{
		if(m_vert)
		{
			vkDestroyShaderModule(p_device->getDevice(), m_shaderModules.at(0).module, nullptr);
		}
		if(m_frag)
		{
			vkDestroyShaderModule(p_device->getDevice(), m_shaderModules.at(1).module, nullptr);
		}
	}

	void GraphicsPipelineBuilder::addVertexShader(const std::string& filepath)
	{
		const auto shaderCode = bs::readFile(filepath);
		VkShaderModule shaderModule = createShaderModule(shaderCode, p_device->getDevice());

		auto& stageInfo = m_shaderModules.at(0);
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		stageInfo.module = shaderModule;
		stageInfo.pName = shaderName;

		m_vert = true;
	}

	void GraphicsPipelineBuilder::addFragmentShader(const std::string& filepath)
	{
		const auto shaderCode = bs::readFile(filepath);
		VkShaderModule shaderModule = createShaderModule(shaderCode, p_device->getDevice());
		
		auto& stageInfo = m_shaderModules.at(1);
		stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stageInfo.module = shaderModule;
		stageInfo.pName = shaderName;

		m_frag = true;
	}

	void GraphicsPipelineBuilder::useVertexDescription(const VertexInputDescription& vdesc)
	{
		m_vertex_description = vdesc;
	}

	void GraphicsPipelineBuilder::setPushConstantSize(const size_t sizeofStructInBytes)
	{
		constexpr auto MAX_PUSH_CONSTANT_SIZE_BYTES = 128;

		pushConstantStructSize = std::clamp(sizeofStructInBytes, 0ULL, (size_t)MAX_PUSH_CONSTANT_SIZE_BYTES);
		if(pushConstantStructSize != sizeofStructInBytes)
		{
			std::cout << "Push Constant Size is not valid! Max is " << MAX_PUSH_CONSTANT_SIZE_BYTES << " while "
				<< sizeofStructInBytes << " was passed!\n";
		}
		assert(pushConstantStructSize <= MAX_PUSH_CONSTANT_SIZE_BYTES);
	}
	
	void GraphicsPipelineBuilder::setRasterizingData(const bool drawClockwise, const bool cullBack)
	{
		if(drawClockwise)
		{
			m_rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		}
		else
		{
			m_rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		}

		if(cullBack)
		{
			m_rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
		}
		else
		{
			m_rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		}
	}
	
	void GraphicsPipelineBuilder::setDrawMode(DrawMode drawMode)
	{
		switch (drawMode)
		{
		case DrawMode::FILL:	
			m_rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			return;
		case DrawMode::LINE:
			m_rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
			return;
		case DrawMode::POINT:
			m_rasterizer.polygonMode = VK_POLYGON_MODE_POINT;
			return;
		}
	}

	void GraphicsPipelineBuilder::build()
	{
		if(!m_vert || !m_frag)
		{
			std::cerr << "Shaders not built!!!\n";
			throw std::runtime_error("Shaders not built\n");
		}

		//Run the stuff to build the pipeline + layout
		isBuilt = true;

		buildLayout();
		buildPipeline();
	}

	void GraphicsPipelineBuilder::getResults(VkPipeline& pipeline, VkPipelineLayout& pipelineLayout)
	{
		pipeline = m_pipeline;
		pipelineLayout = m_layout;
	}

	VkExtent2D GraphicsPipelineBuilder::getExtent() const
	{
		return VkExtent2D
		{ 
			.width = viewportwidth,
			.height = viewportheight,
		};
	}

	VkViewport GraphicsPipelineBuilder::getViewport() const
	{
		return VkViewport
		{
			.x = 0.0f,
			.y = 0.0f,
			.width = static_cast<float>(getExtent().width),
			.height = static_cast<float>(getExtent().height),
			.minDepth = 0.0f,
			.maxDepth = 1.0f,
		};
	}

	VkRect2D GraphicsPipelineBuilder::getScissor() const
	{
		return VkRect2D
		{
			.offset = { 0, 0 },
			.extent = getExtent(),
		};
	}

	void GraphicsPipelineBuilder::buildLayout()
	{
		VkPushConstantRange constants{};
		constants.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
		constants.offset = 0;

		if(pushConstantStructSize > 0)
		{
			constants.size = pushConstantStructSize;
			m_layoutinfo.pushConstantRangeCount = 1;
			m_layoutinfo.pPushConstantRanges = &constants;
		}
		else
		{
			constants.size = 0;
			m_layoutinfo.pushConstantRangeCount = 0;
			m_layoutinfo.pPushConstantRanges = nullptr;
		}

		VkResult result = vkCreatePipelineLayout(p_device->getDevice(), &m_layoutinfo, nullptr, &m_layout);
		if(result != VK_SUCCESS) 
		{
			std::cerr << "Failed to create pipeline layout, error code: " << result << "\n";
			throw std::runtime_error("Failed to create pipeline layout!");
		}

		m_gfxCreateInfo.layout = m_layout;
	}

	void GraphicsPipelineBuilder::buildPipeline()
	{
		//Shader Stages Stuff
		m_gfxCreateInfo.stageCount = m_shaderModules.size();
		m_gfxCreateInfo.pStages = m_shaderModules.data();

		//Dynamic State
		std::vector<VkDynamicState> dynamicStateEnables = 
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynState = {};
		dynState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynState.pNext = nullptr;
		dynState.dynamicStateCount = static_cast<u32>(dynamicStateEnables.size());
		dynState.pDynamicStates = dynamicStateEnables.data();
		m_gfxCreateInfo.pDynamicState = &dynState;

		//Viewport / Scissor State Stuff
		const VkViewport viewport = getViewport();
		const VkRect2D scissor = getScissor();
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.pNext = nullptr;
		viewportState.flags = 0;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;
		m_gfxCreateInfo.pViewportState = &viewportState;

		//Color Blend State
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.pNext = nullptr;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;
		m_gfxCreateInfo.pColorBlendState = &colorBlending;

		//Multisample State
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		m_gfxCreateInfo.pMultisampleState = &multisampling;

		//Input Assembly
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.pNext = nullptr;
		inputAssembly.flags = 0;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		m_gfxCreateInfo.pInputAssemblyState = &inputAssembly;

		//Vertex Description
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.pNext = nullptr;
		vertexInputInfo.vertexBindingDescriptionCount = (u32)m_vertex_description.bindings.size();
		vertexInputInfo.vertexAttributeDescriptionCount = (u32)m_vertex_description.attributes.size();
		vertexInputInfo.pVertexBindingDescriptions = m_vertex_description.bindings.data();
		vertexInputInfo.pVertexAttributeDescriptions = m_vertex_description.attributes.data();
		m_gfxCreateInfo.pVertexInputState = &vertexInputInfo;


		VkPipelineDepthStencilStateCreateInfo depthCreateInfo{};
		depthCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthCreateInfo.pNext = nullptr;
		depthCreateInfo.depthTestEnable = VK_TRUE;
		depthCreateInfo.depthWriteEnable = VK_TRUE;
		depthCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL; //enableDepthTest ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_ALWAYS;
		depthCreateInfo.depthBoundsTestEnable = VK_FALSE;
		depthCreateInfo.minDepthBounds = 0.0f;
		depthCreateInfo.maxDepthBounds = 1.0f;
		depthCreateInfo.stencilTestEnable = VK_FALSE;
		m_gfxCreateInfo.pDepthStencilState = &depthCreateInfo;

		//Creation of the pipeline
		VkResult result = vkCreateGraphicsPipelines(p_device->getDevice(), VK_NULL_HANDLE, 1, &m_gfxCreateInfo, nullptr, &m_pipeline);
		if(result != VK_SUCCESS)
		{
			std::cerr << "Failed to create graphics pipeline, the result is: " << result << "\n";
			throw std::runtime_error("Failed to create graphics pipeline!");
		}
	}
}