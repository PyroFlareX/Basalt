#pragma once

#include "GeneralRenderer.h"
#include "UIRenderer.h"
#include "ChunkRenderer.h"

class Camera;

class Renderer
{
public:
	Renderer(bs::Device* device, VkRenderPass genericPass);
	~Renderer();

	void drawObject(const bs::GameObject& entity);
	//NOT DONE DO NOT USE
	void drawText();
	//Create Chunk Draw Lists
	void recreateChunkDrawLists();
	void passChunkMeshGenerator(const void* chunk_mesh_manager);

	//Pass tell the subrenderers to generate list queues
	void render(Camera& cam);

	//Render to the Framebuffer
	void finish(bs::vk::FramebufferData& fbo, int index);

	//Empty drawqueues
	void clearQueue();

	std::unique_ptr<GeneralRenderer>	m_generalRenderer;
	std::unique_ptr<ChunkRenderer>		m_chunkRenderer;
	std::unique_ptr<UIRenderer>			m_UIRenderer;
		
	//Push the uniform buffer and image descriptor to the gpu
	void pushGPUData(Camera& cam);

private:
	void initGUI();

	//Info for descriptors; as struct
	struct DescriptorSetInfo
	{
		VkDescriptorType	type;
		u32 				bindingSlot;
		u32					count;
	};

	//Helper Functions for init
	
	void initCommandPoolAndBuffers();
	
	void initDescriptorPool(const std::vector<DescriptorSetInfo>& sets);
	void initDescriptorSets(const std::vector<DescriptorSetInfo>& sets);

	void initDescriptorSetBuffers(const std::vector<DescriptorSetInfo>& sets);

	//Vulkan Stuff
	VkRenderPass m_renderpassdefault;
	std::vector<VkCommandBuffer> m_primaryBuffers;
	VkCommandPool m_pool;

	bs::Device* device;

	//Descriptor Handle Stuff
	VkDescriptorPool m_descpool;
	VkDescriptorSet m_descsetglobal;
	VkDescriptorSetLayout desclayout;
};