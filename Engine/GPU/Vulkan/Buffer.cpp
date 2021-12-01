#include "Buffer.h"

#include <stdexcept>
#include <memory>

namespace bs::vk
{
	Buffer::Buffer(const BufferDescription bufdesc) : m_desc(bufdesc)
	{
		if(m_desc.stride == 0)
		{
			m_desc.stride = 1;
		}

		// allocateBuffer();
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = m_desc.usage;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = getSize();
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// To get the buffer type
		if (m_desc.bufferType & bs::vk::BufferUsage::VERTEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::INDEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::UNIFORM_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::STORAGE_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::INDIRECT_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::TRANSFER_BUFFER)
		{
			if(m_desc.usage == VMA_MEMORY_USAGE_GPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
			else if(m_desc.usage == VMA_MEMORY_USAGE_CPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			}
			else
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
		}
		
		vmaCreateBuffer(m_desc.dev->getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

		if(m_desc.bufferData != nullptr && m_desc.usage != VMA_MEMORY_USAGE_GPU_ONLY)
		{
			writeBuffer(m_desc.bufferData);
			m_desc.bufferData = nullptr;
		}
	}

	Buffer::~Buffer()
	{
		deleteBuffer();
	}

	u64 Buffer::getStride() const
	{
		return m_desc.stride;
	}

	u64 Buffer::getSize() const
	{
		return m_desc.size;
	}

	u64 Buffer::getNumElements() const
	{
		return m_desc.size / m_desc.stride;
	}

	void Buffer::setMaxElements(u64 numElements)
	{
		m_desc.size = m_desc.stride * numElements;
	}

	void Buffer::setAllocationSize(const u64 numBytes)
	{
		m_desc.size = numBytes;
	}
	
	void Buffer::allocateBuffer()
	{
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = m_desc.usage;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = getSize();
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// To get the buffer type
		if (m_desc.bufferType & bs::vk::BufferUsage::VERTEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::INDEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::UNIFORM_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::STORAGE_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::INDIRECT_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		if (m_desc.bufferType & bs::vk::BufferUsage::TRANSFER_BUFFER)
		{
			if(m_desc.usage == VMA_MEMORY_USAGE_GPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
			else if(m_desc.usage == VMA_MEMORY_USAGE_CPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			}
			else
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
		}
		
		deleteBuffer();
		vmaCreateBuffer(m_desc.dev->getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

		if(m_desc.bufferData != nullptr && m_desc.usage != VMA_MEMORY_USAGE_GPU_ONLY)
		{
			writeBuffer(m_desc.bufferData);
			m_desc.bufferData = nullptr;
		}
	}

	void Buffer::writeBuffer(const void* data, u64 size, u64 offset)
	{
		if(size == 0)
		{
			size = getSize();
		}

		//Maps GPU memory to CPU visible address
		void* bufferdata = nullptr;
		vmaMapMemory(m_desc.dev->getAllocator(), m_allocation, &bufferdata);
		//Add the offset
		bufferdata = (u8*)bufferdata + offset;
		memcpy(bufferdata, data, size);

		//Unmaps the buffer
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

	VmaAllocation& Buffer::getAllocation()
	{
		return m_allocation;
	}

	void Buffer::deleteBuffer()
	{
		vmaDestroyBuffer(m_desc.dev->getAllocator(), m_buffer, m_allocation);
	}
}