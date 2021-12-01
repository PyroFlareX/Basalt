#define NOMINMAX

#include "UIRenderer.h"

#include <GPU/Vulkan/PipelineBuilder.h>
#include <Engine.h>

UIRenderer::UIRenderer(bs::Device* device, VkRenderPass& rpass, VkDescriptorSetLayout desclayout)	:	
	m_renderpass(rpass), p_device(device)
{
	//Create the command pool
	bs::vk::createCommandPool(*p_device, m_pool);

	//Number of cmd buffers to have
	m_renderlist.resize(1);

	const VkCommandBufferAllocateInfo allocInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
		.pNext = nullptr,
		.commandPool = m_pool,
		.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY,
		.commandBufferCount = (u32)m_renderlist.size(),
	};

	if(vkAllocateCommandBuffers(p_device->getDevice(), &allocInfo, m_renderlist.data()) != VK_SUCCESS) 
	{
		throw std::runtime_error("Failed to allocate command buffers!");
	}

	//Cmd buffer Inheritance info init
	m_inheritanceInfo = VkCommandBufferInheritanceInfo
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO,
		.pNext = nullptr,
		.renderPass = m_renderpass,
		.subpass = 0,
		// Set to Null because the finish render function has the target frame buffer to render to
		.framebuffer = VK_NULL_HANDLE,
		.occlusionQueryEnable = 0,
		.queryFlags = 0,
		.pipelineStatistics = 0
	};

	//Cmd buffer starting info
	m_beginInfo = VkCommandBufferBeginInfo 
	{
		.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
		.pNext = nullptr,
		.flags = VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
		.pInheritanceInfo = &m_inheritanceInfo,
	};
	
	/*bs::vk::GraphicsPipelineBuilder graphicsPipelineBuilder(p_device, m_renderpass, desclayout);
	graphicsPipelineBuilder.addVertexShader("res/Shaders/uivert.spv");
	graphicsPipelineBuilder.addFragmentShader("res/Shaders/uifrag.spv");
	graphicsPipelineBuilder.setDrawMode(bs::vk::DrawMode::FILL);
	graphicsPipelineBuilder.setRasterizingData(false, true);
	graphicsPipelineBuilder.setPushConstantSize(sizeof(PushConstantsIMGUI));
	graphicsPipelineBuilder.useVertexDescription(bs::vk::getVertexDescriptionImGUI());

	graphicsPipelineBuilder.build();
	graphicsPipelineBuilder.getResults(m_genericPipeline, m_pipelineLayout);*/

	bs::vk::createUIPipeline(*p_device, m_gui_pipeline, m_renderpass, m_gui_layout, desclayout);
}

UIRenderer::~UIRenderer()
{
	// GUI/ImGui related
	vkDestroyPipeline(p_device->getDevice(), m_gui_pipeline, nullptr);
	vkDestroyPipelineLayout(p_device->getDevice(), m_gui_layout, nullptr);

	//Cmd buffers
	vkDestroyCommandPool(p_device->getDevice(), m_pool, nullptr);
}

void UIRenderer::addText(const std::string& text, bs::vec2i screenSpacePosition)
{
	m_text.emplace_back(text, screenSpacePosition);
}

void UIRenderer::ImGuiRender()
{	
	drawText();
}

void UIRenderer::render()
{	
	const auto& io = ImGui::GetIO();
	const VkViewport vport
	{
		.x = 0.0f,
		.y = 0.0f,
		.width = io.DisplaySize.x,
		.height = io.DisplaySize.y,
		.minDepth = 0.0f,
		.maxDepth = 1.0f
	};

	vkResetCommandPool(p_device->getDevice(), m_pool, VK_COMMAND_POOL_RESET_RELEASE_RESOURCES_BIT);
	auto& cmd = m_renderlist.at(0);

	if(vkBeginCommandBuffer(cmd, &m_beginInfo) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to begin recording command buffer!");
	}

	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_gui_pipeline);
	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, m_gui_layout, 0, 1, bs::asset_manager->pDescsetglobal, 0, nullptr);

	// UI scale and translate via push constants
	vkCmdSetViewport(cmd, 0, 1, &vport);

	PushConstantsIMGUI pushconstantblock
	{
		.scale = bs::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y),
		.translate = bs::vec2(-1.0f),
		.textureID = bs::vec4(0.0f),
	};

	const auto* drawData = ImGui::GetDrawData();
	int vertexOffset = 0;
	int indexOffset = 0;

	if(drawData->CmdListsCount > 0)
	{
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmd, 0, 1, &bs::asset_manager->getBuffer("GUIvert")->getAPIResource(), &offset);
		vkCmdBindIndexBuffer(cmd, bs::asset_manager->getBuffer("GUIindex")->getAPIResource(), offset, VK_INDEX_TYPE_UINT16);

		for (int i = 0; i < drawData->CmdListsCount; ++i)
		{
			const ImDrawList* cmd_list = drawData->CmdLists[i];
			
			for(int j = 0; j < cmd_list->CmdBuffer.Size; ++j)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];

				const VkRect2D scissorRect
				{
					.offset = 
					{
						.x = std::max((int)(pcmd->ClipRect.x), 0), 
						.y = std::max((int)(pcmd->ClipRect.y), 0)
					},
					.extent = 
					{
						.width = (u32)(pcmd->ClipRect.z - pcmd->ClipRect.x),
						.height = (u32)(pcmd->ClipRect.w - pcmd->ClipRect.y)
					}
				};

				vkCmdSetScissor(cmd, 0, 1, &scissorRect);
				
				if(pcmd->TextureId != nullptr)
				{
					void* t_id = (pcmd->TextureId);
					pushconstantblock.textureID.x = static_cast<float>(reinterpret_cast<u64>(t_id));
				}

				vkCmdPushConstants(cmd, m_gui_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantsIMGUI), &pushconstantblock);
						
				vkCmdDrawIndexed(cmd, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
				indexOffset += pcmd->ElemCount;
				pushconstantblock.textureID = bs::vec4(0.0f);
			}
			
			vertexOffset += cmd_list->VtxBuffer.Size;
		}
	}

	if(vkEndCommandBuffer(cmd) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to record command buffer!");
	}
}

void UIRenderer::executeCommands(VkCommandBuffer cmd)
{
	vkCmdExecuteCommands(cmd, 1, m_renderlist.data());
}

void UIRenderer::bakeImGui()
{
	//Draw IMGUI stuff | temp, move to UIRenderer eventually | or maybe keep here, there's no reason to move it? idk
	const auto* drawData = ImGui::GetDrawData();

	auto vertexBuffer = bs::asset_manager->getBuffer("GUIvert");
	auto indexBuffer = bs::asset_manager->getBuffer("GUIindex");

	const u32 vertSize = drawData->TotalVtxCount;
	const u32 indexSize = drawData->TotalIdxCount;

	//Checks if the buffers need to be recreated |	if the num of verticies or indices is more than 
	//												the amount in the buffer, then reallocated buffers
	if((vertSize > vertexBuffer->getNumElements()) || (indexSize > indexBuffer->getNumElements()))
	{
		vertexBuffer->setMaxElements(vertSize);
		indexBuffer->setMaxElements(indexSize);

		vertexBuffer->allocateBuffer();
		indexBuffer->allocateBuffer();
	}
	
	u32 offsetvert = 0;
	u32 offsetindex = 0;
	for(int n = 0; n < drawData->CmdListsCount; ++n)
	{
		const ImDrawList* cmdlist = drawData->CmdLists[n];
		vertexBuffer->writeBuffer(cmdlist->VtxBuffer.Data, cmdlist->VtxBuffer.size_in_bytes(), offsetvert);
		indexBuffer->writeBuffer(cmdlist->IdxBuffer.Data, cmdlist->IdxBuffer.size_in_bytes(), offsetindex);

		offsetvert += cmdlist->VtxBuffer.size_in_bytes();
		offsetindex += cmdlist->IdxBuffer.size_in_bytes();
	}
}

void UIRenderer::drawText()
{
	//clears imgui window background
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

	if(ImGui::Begin("Text", 0, ImGuiWindowFlags_NoDecoration))
	{
		if(m_text.size() == 0)
		{
			ImGui::End();

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();

			return;
		}

		auto* drawlist = ImGui::GetWindowDrawList();
	
		for(const auto& text : m_text)
		{	//Can make this better later, just gotta fix a bunch of math
			//Also using manual ImDrawList should be more efficient for this usecase

			drawlist->AddText(ImVec2(text.pixelPos.x, text.pixelPos.y), IM_COL32_WHITE, text.text.c_str());
			
			//ImGui::SetWindowPos(ImVec2(text.pixelPos.x, text.pixelPos.y));

			//ImGui::Text(text.text.c_str());
		}
	}
	ImGui::End();

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
}