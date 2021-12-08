#pragma once

#include <string>
#include <vector>
#include <Types/Types.h>
#include <GPU/GPU.h>


class UIRenderer
{
public:
	UIRenderer(bs::Device* device, VkRenderPass& rpass, VkDescriptorSetLayout desclayout);
	~UIRenderer();

	//Add text to be rendered //screenSpacePosition is for top right
	void addText(const std::string& text, bs::vec2i screenSpacePosition);

	//Render the text into the cmd buffers
	void ImGuiRender();

	void render();

	void executeCommands(VkCommandBuffer cmd);

	void bakeImGui();
private:

	void drawText();

	typedef struct RenderText_t
	{
		std::string text;
		bs::vec2i pixelPos;
	} render_text;

	std::vector<render_text> m_text;

	// Pipeline Stuff
	VkPipelineLayout m_gui_layout;
	VkPipeline m_gui_pipeline;
	VkRenderPass& m_renderpass;

	//For Command Buffers
	VkCommandBufferBeginInfo m_beginInfo;
	VkCommandBufferInheritanceInfo m_inheritanceInfo;
	
	// Vulkan Stuff
	VkCommandPool m_pool;
	std::vector<VkCommandBuffer> m_renderlist;
	bs::Device* p_device;
};