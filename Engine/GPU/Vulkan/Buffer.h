#pragma once

#include "../../Resources/Mesh.h"

#include "Device.h"
#include "VulkanHelpers.h"
#include <stdexcept>
#include <vulkan/vulkan_core.h>


struct BufferDescription
{
    vn::Mesh m_mesh;

    vn::Device dev;
};


class Buffer
{
public:
    Buffer(BufferDescription bufdesc)
    {
        m_desc = bufdesc;

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size =sizeof(vn::vec3) * bufdesc.m_mesh.vertices.size();

        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        

        if(vkCreateBuffer(bufdesc.dev.getDevice(), &bufferInfo, nullptr, &m_buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("BUFFER CREATION FAILED");
        }
    }

    unsigned int getStride();
    unsigned int getSize();
    unsigned int getNumElements();

    void setAPIResource(VkBuffer buffer);
    VkBuffer getAPIResource();

    ~Buffer()
    {
        vkDestroyBuffer(m_desc.dev.getDevice(), m_buffer, nullptr);
    }
private:
    VkBuffer m_buffer;

    BufferDescription m_desc;
};