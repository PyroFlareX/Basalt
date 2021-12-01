#include "Buffer.h"

namespace bs::vk
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
		return m_desc.size;
	}

	size_t Buffer::getNumElements()
	{
		return m_desc.size / m_desc.stride;
	}

	void Buffer::uploadBuffer(bool write)
	{
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = getSize();

		bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		// To get the buffer type
		if (m_desc.bufferType == bs::vk::BufferUsage::VERTEX_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		else if (m_desc.bufferType == bs::vk::BufferUsage::INDEX_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		else if (m_desc.bufferType == bs::vk::BufferUsage::UNIFORM_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		else if (m_desc.bufferType == bs::vk::BufferUsage::STORAGE_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		else if (m_desc.bufferType == bs::vk::BufferUsage::INDIRECT_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		else if (m_desc.bufferType == bs::vk::BufferUsage::TRANSFER_BUFFER)
		{
			bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			allocInfo.usage = VMA_MEMORY_USAGE_CPU_ONLY;
		}
		
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		
		vmaCreateBuffer(m_desc.dev->getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

		if(write)
		{
			writeBuffer(m_desc.bufferData);
		}
	}

	void Buffer::writeBuffer(void* data, size_t size, size_t offset)
	{
		void* bufferdata;
		
		if(size == 0)
		{
			size = getSize();
		}

		//Maps GPU memory to CPU visible address
		vmaMapMemory(m_desc.dev->getAllocator(), m_allocation, &bufferdata);

		bufferdata = reinterpret_cast<char*>(bufferdata) + offset;
		
		memcpy(bufferdata, data, size);

		vmaUnmapMemory(m_desc.dev->getAllocator(), m_allocation);
	}

	void Buffer::setAPIResource(VkBuffer& buffer)
	{
		deleteBuffer();
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

	void Buffer::setMaxElements(size_t numElements)
	{
		m_desc.size = m_desc.stride * numElements;
	}
}