#include "Buffer.h"

namespace vn::vk
{
	Buffer::Buffer(BufferDescription bufdesc)
    {
        m_desc = bufdesc;

        
    }

    Buffer::~Buffer()
    {
        //vkDestroyBuffer(m_desc.dev.getDevice(), m_buffer, nullptr);
    }

    unsigned int Buffer::getStride()
    {
        return sizeof(vn::Vertex);
    }

    unsigned int Buffer::getSize()
    {
        return m_desc.m_mesh.vertices.size();
    }

    unsigned int Buffer::getNumElements()
    {
        return m_desc.m_mesh.indicies.size();
    }

    void Buffer::uploadMesh()
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(vn::vec3) * m_desc.m_mesh.vertices.size();

        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        
        vmaCreateBuffer(m_desc.dev.getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

        void* meshData;
        //Maps GPU memory to CPU visible address
        vmaMapMemory(m_desc.dev.getAllocator(), m_allocation, &meshData);

        memcpy(meshData, m_desc.m_mesh.vertices.data(), m_desc.m_mesh.vertices.size() * sizeof(vn::Vertex));

        vmaUnmapMemory(m_desc.dev.getAllocator(), m_allocation);


        //Index
        VkBufferCreateInfo bufferInfoIndex{};
        bufferInfoIndex.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfoIndex.size = sizeof(unsigned int) * m_desc.m_mesh.indicies.size();

        bufferInfoIndex.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        bufferInfoIndex.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        vmaCreateBuffer(m_desc.dev.getAllocator(), &bufferInfoIndex, &allocInfo, &m_index, &m_indexAlloc, nullptr);

        void* indexData;
        
        //Maps GPU memory to CPU visible address
        vmaMapMemory(m_desc.dev.getAllocator(), m_indexAlloc, &indexData);

        memcpy(indexData, m_desc.m_mesh.indicies.data(), m_desc.m_mesh.indicies.size() * sizeof(unsigned int));

        vmaUnmapMemory(m_desc.dev.getAllocator(), m_indexAlloc);
    }

    void Buffer::setAPIResource(VkBuffer buffer)
    {
        m_buffer = buffer;


    }

    VkBuffer& Buffer::getAPIResource()
    {
        return m_buffer;
    }

}