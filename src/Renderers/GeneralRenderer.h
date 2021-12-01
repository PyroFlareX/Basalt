#pragma once

#include "../Camera.h"
#include <Engine.h>

class GeneralRenderer
{
public:
	GeneralRenderer(bs::Device* mainDevice, VkRenderPass* rpass, VkDescriptorSetLayout desclayout);

	void addInstance(bs::GameObject& entity);
	void render(Camera& cam);

	void clearQueue();

	std::vector<VkCommandBuffer>& getRenderlists();
	
	~GeneralRenderer();
private:
	//Hack bool
	bool firstrun = false;

	std::vector<bs::GameObject> m_queue;
	
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

