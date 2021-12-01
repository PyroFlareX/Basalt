#pragma once

#include "VulkanHelpers.h"

namespace bs::vk
{
	enum class ShaderType
	{
		VERTEX = 0,
		FRAGMENT,
		COMPUTE,
		MESH,
		TASK,
		TESSALATION,
		GEOMETRY,
		NUM_SHADER_TYPES
	};

	enum class DrawMode
	{
		FILL = 0,
		LINE,
		POINT
	};

	class GraphicsPipelineBuilder
	{
	public:
		GraphicsPipelineBuilder(bs::Device* mainDevice, VkRenderPass renderpass, VkDescriptorSetLayout& desclayout);
		~GraphicsPipelineBuilder();

		void addVertexShader(const std::string& filepath);
		void addFragmentShader(const std::string& filepath);
		//Pass a prebuilt Vertex Description
		void useVertexDescription(const VertexInputDescription& vdesc);
		//Pass `sizeof(constantsStruct)` where constantsStruct is the struct being used for Push Constants
		void setPushConstantSize(const size_t sizeofStructInBytes);
		//If drawClockwise = false, draws Counterclockwise
		//If cullBack = true, culls back, else cullsFront
		void setRasterizingData(const bool drawClockwise, const bool cullBack);
		// If 0, draws polys
		// If 1, draws lines
		// If 2, draws points
		void setDrawMode(DrawMode drawMode);
		//Build the Pipeline and Pipeline Layout
		void build();
		//Pass the pipeline and pipelineLayout that the built pipeline should be copied to
		void getResults(VkPipeline& pipeline, VkPipelineLayout& pipelineLayout);
	private:
		bool isBuilt;
		bool m_frag;
		bool m_vert;
		
		size_t pushConstantStructSize;

		//Device Pointer
		bs::Device* p_device;
		//Pipeline
		VkPipeline m_pipeline;
		//Pipeline Create Info
		VkGraphicsPipelineCreateInfo m_gfxCreateInfo;
		//Pipeline layout
		VkPipelineLayout m_layout;
		//Pipeline Layout Info
		VkPipelineLayoutCreateInfo m_layoutinfo;
		//Rasterizer
		VkPipelineRasterizationStateCreateInfo m_rasterizer;
		//Shader Modules
		std::vector<VkPipelineShaderStageCreateInfo> m_shaderModules;

		//Vertex Description
		VertexInputDescription m_vertex_description;
		int numBindings;
	
	private:
		//Private helper functions
		//Return the default extent
		VkExtent2D getExtent() const;
		//Return the default viewport
		VkViewport getViewport() const;
		//Return a default scissor
		VkRect2D getScissor() const;
		//Build the pipeline layout
		void buildLayout();
		//Build the pipeline
		void buildPipeline();
	};
}