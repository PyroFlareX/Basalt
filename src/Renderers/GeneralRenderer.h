#pragma once

#include "../Camera.h"
#include <Engine.h>

class GeneralRenderer
{
public:
	GeneralRenderer(bs::Device* mainDevice, VkRenderPass& rpass, VkDescriptorSetLayout desclayout);
	~GeneralRenderer();

	void addInstance(const bs::GameObject& entity);
	void render(Camera& cam);

	void executeCommands(VkCommandBuffer cmd);

	void clearQueue();

	std::vector<VkCommandBuffer> getRenderlists();
private:
	std::vector<bs::GameObject> m_queue;
	
	// Pipeline Stuff

	VkPipelineLayout m_pipelineLayout;
	VkPipeline m_genericPipeline;
	VkCommandBufferBeginInfo m_beginInfo;
	VkCommandBufferInheritanceInfo m_inheritanceInfo;
	VkRenderPass& m_renderpass;

	// Vulkan Stuff
	
	VkCommandPool m_pool;
	std::vector<VkCommandBuffer> m_renderlist;
	bs::Device* p_device;
};

