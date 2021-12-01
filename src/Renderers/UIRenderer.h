#pragma once

#include <string>
#include <vector>
#include <Types/Types.h>
#include <GPU/Context.h>


class UIRenderer
{
public:
	UIRenderer(bs::Device* device, VkPipeline pipeline, VkPipelineLayout piplineLayout);

	//Add text to be rendered //screenSpacePosition is for top right
	void addText(const std::string& text, bs::vec2i screenSpacePosition);

	//Render the text into the cmd buffers
	void render();

	void finish(VkCommandBuffer& guiBuffer);

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
	VkPipelineLayout playout;
	VkPipeline gfx;
	VkCommandBufferBeginInfo beginInfo{};
	VkCommandBufferInheritanceInfo inheritanceInfo{};
	VkRenderPass* m_renderpass;


	// Vulkan Stuff
	VkCommandPool m_pool;
	std::vector<VkCommandBuffer> m_renderlist;
	bs::Device* p_device;
};