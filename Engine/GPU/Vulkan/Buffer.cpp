#include "Buffer.h"

namespace vn::vk
{
	Buffer::Buffer(BufferDescription bufdesc) : m_desc(bufdesc)
	{

		
	}

	Buffer::~Buffer()
	{
		deleteBuffer();
	}

	size_t Buffer::getStride()
	{
		return m_desc.stride;
	}

	size_t Buffer::getSize()
	{
		return m_desc.size * m_desc.stride;
	}

	size_t Buffer::getNumElements()
	{
		return m_desc.size;
	}

	void Buffer::uploadBuffer()
	{
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = getSize();

		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		// To get the buffer type
		if (m_desc.bufferType == vn::BufferUsage::VERTEX_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		else if (m_desc.bufferType == vn::BufferUsage::INDEX_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		else if (m_desc.bufferType == vn::BufferUsage::UNIFORM_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		else if (m_desc.bufferType == vn::BufferUsage::STORAGE_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		else if (m_desc.bufferType == vn::BufferUsage::INDIRECT_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		
		vmaCreateBuffer(m_desc.dev->getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

		void* bufferdata;
		//Maps GPU memory to CPU visible address
		vmaMapMemory(m_desc.dev->getAllocator(), m_allocation, &bufferdata);

		memcpy(bufferdata, m_desc.bufferData, getSize());

		vmaUnmapMemory(m_desc.dev->getAllocator(), m_allocation);
	}

	void Buffer::writeBuffer(const void* data)
	{
		void* bufferdata;
		//Maps GPU memory to CPU visible address
		vmaMapMemory(m_desc.dev->getAllocator(), m_allocation, &bufferdata);

		memcpy(bufferdata, data, getSize());

		vmaUnmapMemory(m_desc.dev->getAllocator(), m_allocation);
	}

	void Buffer::setAPIResource(VkBuffer& buffer)
	{
		//deleteBuffer();
		m_buffer = buffer;
	}

	VkBuffer& Buffer::getAPIResource()
	{
		return m_buffer;
	}

	void Buffer::deleteBuffer()
	{
		vmaDestroyBuffer(m_desc.dev->getAllocator(), m_buffer, m_allocation);
	}

}