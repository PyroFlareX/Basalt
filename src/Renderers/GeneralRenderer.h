#pragma once

#include "../Camera.h"
#include "../../Engine/Engine.h"

class GeneralRenderer
{
public:
	GeneralRenderer(vn::Device* mainDevice, VkRenderPass* rpass, VkDescriptorSetLayout desclayout);

	void addInstance(vn::GameObject& entity);
	void render(Camera& cam);

	void clearQueue();

	std::vector<VkCommandBuffer>& getRenderlists();
	
	~GeneralRenderer();
private:
	//Hack bool
	bool firstrun = false;

	std::vector<vn::GameObject> m_queue;
	vn::Image img;
	
	// Pipeline Stuff

	VkPipelineLayout playout;
	VkPipeline gfx;
	VkCommandBufferBeginInfo beginInfo{};
	VkCommandBufferInheritanceInfo inheritanceInfo{};
	VkRenderPass* m_renderpass;


	// Vulkan Stuff
	
	VkCommandPool m_pool;
	std::vector<VkCommandBuffer> m_renderlist;
	vn::Device* p_device;

};

