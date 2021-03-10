#pragma once

#include "../Camera.h"
#include "../../Engine/Engine.h"

class GeneralRenderer {
public:
	GeneralRenderer(vn::Device *mainDevice, VkRenderPass *rpass);

	void addInstance(vn::GameObject &entity);

	void render(Camera &cam);

	void clearQueue();

	std::vector<VkCommandBuffer> &getRenderlists();

	~GeneralRenderer();

private:
	//Hack bool
	bool firstrun = false;

	std::vector<vn::GameObject> m_queue;
	vn::Image img;

	//Buffers
	std::vector<vn::vk::Model *> m_models;
	//vn::vk::Buffer m_descriptorbuffer;
	std::vector<vn::vk::Buffer *> m_descriptorBuffers;

	// Pipeline Stuff
	VkPipelineLayout playout;
	VkPipeline gfx;
	VkCommandBufferBeginInfo beginInfo{};
	VkCommandBufferInheritanceInfo inheritanceInfo{};
	VkRenderPass *m_renderpass;

	VkDescriptorPool m_descpool;
	VkDescriptorSet m_descsetglobal;

	// Vulkan Stuff
	VkCommandPool m_pool;
	std::vector<VkCommandBuffer> m_renderlist;
	vn::Device *p_device;

};

