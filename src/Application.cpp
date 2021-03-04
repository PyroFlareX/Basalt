#include "Application.h"

#include "Camera.h"
#include "States/Gamestate.h"

#include <unordered_map>

Application::Application() : m_camera(0)
{
	pushState(std::make_unique<GameState>(*this));
	setModeVR(false);

	// Needed for setup
	m_context.setDeviceptr(&m_device);
	m_context.initAPI();

	m_renderer = new Renderer(&m_device);
}

void Application::setModeVR(bool haveVR) noexcept
{
	VRmode = haveVR;
}

void Application::RunLoop()
{
	//Initial Utilities Setup
	vn::Clock clock;
	float t = 0;
	double dt = 0;
	float frames = 0.0f;

	//Refresh Window
	//m_context.clear();
	//m_context.update();

	vn::vec2 winSize = vn::vec2(800.0f, 600.0f);
	
	
//===================================================================================
	
	/// TODO: Create 2nd half of the Vulkan wrapper, like Renderpasses, Pipelines,

	VkRenderPass rpass;
	vn::vk::createRenderPass(m_device.getDevice(), m_context.m_scdetails, rpass);
	/*
	VkPipelineLayout playout;
	VkPipeline gfx;
	vn::vk::createGraphicsPipeline(rpass, m_context.m_scdetails, playout, gfx, m_device.getDevice());
	*/
	vn::vk::createFramebuffers(rpass, m_context.m_scdetails, m_device.getDevice());

	vn::vk::FramebufferData framebufdata{};
	framebufdata.handle = m_context.m_scdetails.swapChainFramebuffers;
	framebufdata.imgView = m_context.m_scdetails.swapChainImageViews.at(0);
	framebufdata.size = winSize;

	//m_context.m_scdetails.


	//VkCommandPool cpoolp;
	//vn::vk::createCommandPool(m_device, cpoolp);
	/*
	std::unordered_map<uint8_t, VkCommandPool> cmdpool;
	for (int i = 0; i < jobSystem.numThreads(); i++)
	{
		VkCommandPool cpool;
		vn::vk::createCommandPool(m_device, cpool);
		cmdpool[i] = cpool;
	}
	

	std::vector<VkCommandBuffer> secondary(cmdpool.size() - 1);
	for (int i = 1; i < cmdpool.size(); i++)
	{
		vn::vk::createSecondaryCommandBuffers(m_device.getDevice(), cmdpool[i], m_context.m_scdetails, gfx, rpass, secondary[i - 1]);
	}

	std::vector<VkCommandBuffer> primary;
	vn::vk::createCommandBuffers(m_device.getDevice(), cmdpool[0], m_context.m_scdetails, gfx, rpass, primary, secondary);

	//vn::vk::RenderTargetFramebuffer framebuffer(m_device, rpass, winSize);
	*/

/*
	vn::vk::BufferDescription bufferdesc = {};
	bufferdesc.dev = m_device;

	vn::Mesh mesh;
	vn::Vertex vert;
	
	vert.position = vn::vec3(-0.5f, 0.5f, 0.6f);
	mesh.vertices.push_back(vert);

	vert.position = vn::vec3(0.5f, 0.5f, 0.6f);
	mesh.vertices.push_back(vert);

	vert.position = vn::vec3(0.0f, -0.5f, 0.6f);
	mesh.vertices.push_back(vert);

	mesh.indicies.push_back(0);
	mesh.indicies.push_back(1);
	mesh.indicies.push_back(2);
	//bufferdesc.m_mesh = mesh;
	bufferdesc.m_mesh = vn::loadMeshFromObj("res/Models/sphere.obj");
	vn::vk::Buffer buffer(bufferdesc);
	buffer.uploadMesh();
	

	void* dataBufferRecord[] = { &secondary, &m_context.m_scdetails, &rpass, &gfx, &primary, &cmdpool, &m_device.getDevice(), &this->getCam(), &playout, &buffer };

	Job recordbufferSecondary = jobSystem.createJob([](Job job)
		{
			vkResetCommandPool(*reinterpret_cast<VkDevice*>(job.data[6]),
					reinterpret_cast<std::unordered_map<uint8_t, VkCommandPool>*>(job.data[5])->find(1)->second, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

			VkCommandBuffer buffer = reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0);
			
			Camera camtemp = *reinterpret_cast<Camera*>(job.data[7]);
			vn::vk::Buffer* buffermesh = reinterpret_cast<vn::vk::Buffer*>(job.data[9]);

			PushConstantsStruct pushconst{};
			pushconst.proj = camtemp.getProjMatrix();
			pushconst.view = camtemp.getViewMatrix();
			vn::Transform t;
			
			pushconst.model = vn::makeModelMatrix(t);

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT; // Optional

			VkCommandBufferInheritanceInfo inheritanceInfo{};
			inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
			inheritanceInfo.renderPass = *reinterpret_cast<VkRenderPass*>(job.data[2]);
			// Secondary command buffer also use the currently active framebuffer
			inheritanceInfo.framebuffer = VK_NULL_HANDLE;//reinterpret_cast<SwapChainDetails*>(job.data[1])->swapChainFramebuffers[0];

			beginInfo.pInheritanceInfo = &inheritanceInfo; // Optional

			if (vkBeginCommandBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), &beginInfo) != VK_SUCCESS) {
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			vkCmdBindPipeline(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), VK_PIPELINE_BIND_POINT_GRAPHICS,
							*reinterpret_cast<VkPipeline*>(job.data[3]));

			vkCmdPushConstants(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), *reinterpret_cast<VkPipelineLayout*>(job.data[8]), 
								VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantsStruct), &pushconst);

			VkDeviceSize offset = 0;
			vkCmdBindVertexBuffers(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), 0, 1, 
								&buffermesh->getAPIResource(), &offset);

			//vkCmdBindIndexBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), buffermesh->m_index, offset, VK_INDEX_TYPE_UINT32);

			//vkCmdDrawIndexed(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), buffermesh->getNumElements(), 1, 0, 0, 0);

			//vkCmdDraw(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0), buffermesh->getSize(), 1, 0, 0);

			if (vkEndCommandBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->at(0)) != VK_SUCCESS) {
				throw std::runtime_error("failed to record command buffer!");
			}


		}, dataBufferRecord);

	Job recordbufferPrimary = jobSystem.createJob([](Job job)
		{
			Camera* camtemp = reinterpret_cast<Camera*>(job.data[7]);
			vn::vk::Buffer* buffermesh = reinterpret_cast<vn::vk::Buffer*>(job.data[9]);

			PushConstantsStruct pushconst = {};
			pushconst.proj = camtemp->getProjMatrix();
			pushconst.view = camtemp->getViewMatrix();
			vn::Transform t;
			t.pos.x = 0.0f;
			t.pos.y = 0.0f;
			t.pos.z = 0.0f;
			
			//t.rescale(t, vn::vec3(10.0f, 10.0f, 10.0f));

			pushconst.model = vn::makeModelMatrix(t);
			
			for (size_t i = 0; i < reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->size(); i++) {
				VkCommandBufferBeginInfo beginInfo{};
				beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

				if (vkBeginCommandBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), &beginInfo) != VK_SUCCESS) {
					throw std::runtime_error("failed to begin recording command buffer!");
				}

				VkRenderPassBeginInfo renderPassInfo{};
				renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
				renderPassInfo.renderPass = *reinterpret_cast<VkRenderPass*>(job.data[2]);
				renderPassInfo.framebuffer = reinterpret_cast<SwapChainDetails*>(job.data[1])->swapChainFramebuffers[i];
				renderPassInfo.renderArea.offset = { 0, 0 };
				renderPassInfo.renderArea.extent = reinterpret_cast<SwapChainDetails*>(job.data[1])->swapChainExtent;

				VkClearValue clearColor = { 0.1f, 0.1f, 0.1f, 1.0f };
				renderPassInfo.clearValueCount = 1;
				renderPassInfo.pClearValues = &clearColor;
				//VK_SUBPASS_CONTENTS_INLINE //VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
				vkCmdBeginRenderPass(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

				vkCmdBindPipeline(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), VK_PIPELINE_BIND_POINT_GRAPHICS, *reinterpret_cast<VkPipeline*>(job.data[3]));
				
				VkDeviceSize offset = 0;
				vkCmdBindVertexBuffers(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), 0, 1, 
									&buffermesh->getAPIResource(), &offset);

				vkCmdPushConstants(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), *reinterpret_cast<VkPipelineLayout*>(job.data[8]),
					VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantsStruct), &pushconst);

				vkCmdBindIndexBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), buffermesh->m_index, offset, VK_INDEX_TYPE_UINT32);

				vkCmdDrawIndexed(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), buffermesh->getNumElements(), 1, 0, 0, 0);
				//vkCmdDraw(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), buffermesh->getSize(), 1, 0, 0);
				//vkCmdExecuteCommands(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i), 1 , reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[0])->data());

				vkCmdEndRenderPass(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i));

				if (vkEndCommandBuffer(reinterpret_cast<std::vector<VkCommandBuffer>*>(job.data[4])->at(i)) != VK_SUCCESS) {
					throw std::runtime_error("failed to record command buffer!");
				}
			}
		}, dataBufferRecord);

	void* freecmdbufdata[] = { &primary, &secondary };

	Job freeCmdBuffers = jobSystem.createJob([](Job job)
		{

		}, freecmdbufdata);
	*/
//===================================================================================

	//Main Loop
	Input::window = m_context.getContext();

	//jobSystem.schedule(recordbufferPrimary);
	jobSystem.wait();

    while(m_context.isOpen() && !m_states.empty())
    {
		dt = clock.restart();
		
		
        ///Main Loop, do cycle of Input, Update, Draw, Render & Swap Buffers, Handle Events
		
		///Clear
		m_context.clear();
		m_renderer->clearQueue();
        
		///Input
		currentState().input();
		
        /// Update
		currentState().update(dt);
		currentState().lateUpdate(&m_camera);
		m_camera.update();
		jobSystem.wait();
        /// Draw
		currentState().render(m_renderer);
		jobSystem.wait();

        /// Render

		// Create Command Buffers (and then combine them into Primary Cmd Buffers)

		//jobSystem.schedule(recordbufferSecondary);

		//jobSystem.wait();

		// Submit Work to GPU

		//vkResetCommandPool(m_device.getDevice(), cmdpool.find(0)->second, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
		//jobSystem.schedule(recordbufferPrimary);
		//jobSystem.wait();
		//m_device.submitWork(primary);

		
		m_renderer->render(m_camera);



		m_renderer->finish(framebufdata);
		jobSystem.wait();
		m_context.update();
		

		//vkResetCommandPool(m_device.getDevice(), cmdpool[0], VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
		//vkResetCommandPool(m_device.getDevice(), cmdpool[1], VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);

        /// Handle Window Events
		t += dt;
		frames++;
		if (t >= 1)
		{
			std::cout << frames << " per sec\n";
			std::cout << dt << " ms\n";
			t = 0;
			frames = 0;

			std::cout << "CAMERA X: " << this->getCam().pos.x << "\n";
			std::cout << "CAMERA ROT XY: " << this->getCam().rot.x << " " << this->getCam().rot.y << "\n";
		}
		handleEvents();
    }
	m_context.close();
}



void Application::popState()
{
    m_states.pop_back();
}

Basestate& Application::currentState()
{
    return *m_states.back();
}

void Application::handleEvents()
{
	if (VRmode)
	{
		//Handle VR Events Function
	}

	//Window Events

	
}
