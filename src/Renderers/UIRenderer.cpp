#define NOMINMAX

#include "UIRenderer.h"

#include <Engine.h>
#include <algorithm>

UIRenderer::UIRenderer(bs::Device* device, VkPipeline pipeline, VkPipelineLayout piplineLayout)
{
	p_device = device;
	gfx = pipeline;
	playout = piplineLayout;
}

void UIRenderer::addText(const std::string& text, bs::vec2i screenSpacePosition)
{
	m_text.emplace_back(text, screenSpacePosition);
}

void UIRenderer::render()
{	
	drawText();
	
}

void UIRenderer::finish(VkCommandBuffer& guiBuffer)
{
	auto& cmd = guiBuffer;
	vkCmdBindPipeline(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, gfx);

	vkCmdBindDescriptorSets(cmd, VK_PIPELINE_BIND_POINT_GRAPHICS, playout, 0, 1, bs::asset_manager->pDescsetglobal, 0, nullptr);

	// UI scale and translate via push constants
	auto& io = ImGui::GetIO();

	VkViewport vport;
	vport.x = 0.0f;
	vport.y = 0.0f;
	vport.width = io.DisplaySize.x;
	vport.height = io.DisplaySize.y;
	vport.minDepth = 0.0f;
	vport.maxDepth = 1.0f;

	vkCmdSetViewport(cmd, 0, 1, &vport);

	PushConstantsIMGUI pushconstantblock = {};
	pushconstantblock.scale = bs::vec2(2.0f / io.DisplaySize.x, 2.0f / io.DisplaySize.y);
	pushconstantblock.translate = bs::vec2(-1.0f);
	pushconstantblock.textureID = bs::vec4(0.0f);

	auto* drawData = ImGui::GetDrawData();
	int vertexOffset = 0;
	int indexOffset = 0;

	if(drawData->CmdListsCount > 0)
	{
		
		VkDeviceSize offset = 0;
		vkCmdBindVertexBuffers(cmd, 0, 1, &bs::asset_manager->getBuffer("GUIvert")->getAPIResource(), &offset);
		vkCmdBindIndexBuffer(cmd, bs::asset_manager->getBuffer("GUIindex")->getAPIResource(), offset, VK_INDEX_TYPE_UINT16);

		for (int32_t i = 0; i < drawData->CmdListsCount; ++i)
		{
			const ImDrawList* cmd_list = drawData->CmdLists[i];
			
			for (int32_t j = 0; j < cmd_list->CmdBuffer.Size; ++j)
			{
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[j];

				VkRect2D scissorRect;

				scissorRect.offset.x = std::max((int32_t)(pcmd->ClipRect.x), 0);
				scissorRect.offset.y = std::max((int32_t)(pcmd->ClipRect.y), 0);
				scissorRect.extent.width = (uint32_t)(pcmd->ClipRect.z - pcmd->ClipRect.x);
				scissorRect.extent.height = (uint32_t)(pcmd->ClipRect.w - pcmd->ClipRect.y);
				vkCmdSetScissor(cmd, 0, 1, &scissorRect);
				
				if(pcmd->TextureId != nullptr)
				{
					void* t_id = (pcmd->TextureId);
					pushconstantblock.textureID.x = static_cast<float>(reinterpret_cast<u64>(t_id));
				}

				vkCmdPushConstants(cmd, playout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(PushConstantsIMGUI), &pushconstantblock);
						
				vkCmdDrawIndexed(cmd, pcmd->ElemCount, 1, indexOffset, vertexOffset, 0);
				indexOffset += pcmd->ElemCount;
				pushconstantblock.textureID = bs::vec4(0.0f);
			}
			
			vertexOffset += cmd_list->VtxBuffer.Size;
		}
	}
	if (vkEndCommandBuffer(cmd) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to record command buffer!");
	}

}

void UIRenderer::bakeImGui()
{
	//Draw IMGUI stuff | temp, move to UIRenderer eventually | or maybe keep here, there's no reason to move it? idk
	auto* drawData = ImGui::GetDrawData();

	//Folding scope for recreating the buffers for ImGUI
	{
		bs::vk::Buffer* vertexBuffer = bs::asset_manager->getBuffer("GUIvert");
		bs::vk::Buffer* indexBuffer = bs::asset_manager->getBuffer("GUIindex");

		int vertSize = drawData->TotalVtxCount;
		int indexSize = drawData->TotalIdxCount;

		//Checks if the buffers need to be recreated |	if the num of verticies or indices is more than 
		//												the amount in the buffer, then reallocated buffers
		if((vertSize > vertexBuffer->getNumElements()) || (indexSize > indexBuffer->getNumElements()))
		{
			
			vertexBuffer->deleteBuffer();
			indexBuffer->deleteBuffer();

			vertexBuffer->setMaxElements(vertSize);
			indexBuffer->setMaxElements(indexSize);

			vertexBuffer->uploadBuffer(false);
			indexBuffer->uploadBuffer(false);
		}
		
		{
			int offsetvert = 0;
			int offsetindex = 0;
			for(int n = 0; n < drawData->CmdListsCount; ++n)
			{
				ImDrawList* cmdlist = drawData->CmdLists[n];
				vertexBuffer->writeBuffer(cmdlist->VtxBuffer.Data, cmdlist->VtxBuffer.size_in_bytes(), offsetvert);
				indexBuffer->writeBuffer(cmdlist->IdxBuffer.Data, cmdlist->IdxBuffer.size_in_bytes(), offsetindex);

				offsetvert += cmdlist->VtxBuffer.size_in_bytes();
				offsetindex += cmdlist->IdxBuffer.size_in_bytes();
			}
		}

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