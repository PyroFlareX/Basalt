#pragma once

#include "../Camera.h"
#include "../../Engine/Engine.h"

class GeneralRenderer
{
public:
	GeneralRenderer(vn::Device* mainDevice, VkRenderPass* rpass);

	void addInstance(vn::GameObject& entity);
	void render(Camera& cam);

	void clearQueue();

	std::vector<VkCommandBuffer> getRenderlists() const;
	
	~GeneralRenderer();
private:
	//Hack bool
	bool firstrun = false;

	std::vector<vn::GameObject> m_queue;
	
	VkCommandPool m_pool;
	std::vector<VkCommandBuffer> m_renderlist;
	vn::Device* p_device;

	vn::Image img;

	VkCommandBufferBeginInfo beginInfo{};
	VkCommandBufferInheritanceInfo inheritanceInfo{};
	VkRenderPass* m_renderpass;
};

