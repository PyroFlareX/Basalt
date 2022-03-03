#include "Buffer.h"

#include "Device.h"

#include <stdexcept>
#include <memory>

namespace bs::vk
{
	Buffer::Buffer(const BufferDescription bufdesc) : p_device(bufdesc.dev), size(bufdesc.size),
		stride(bufdesc.stride), bufferType(bufdesc.bufferType), usage(bufdesc.usage)
	{
		m_buffer = VK_NULL_HANDLE;
		m_allocation = VK_NULL_HANDLE;

		if(stride == 0)
		{
			stride = 1;
		}
		
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = usage;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = getSize();
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// To get the buffer type
		if (bufferType & bs::vk::BufferUsage::VERTEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::INDEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::UNIFORM_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::STORAGE_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::INDIRECT_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::TRANSFER_BUFFER)
		{
			if(usage == VMA_MEMORY_USAGE_GPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
			else if(usage == VMA_MEMORY_USAGE_CPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			}
			else
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
		}
		
		vmaCreateBuffer(p_device->getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

		if(bufferData != nullptr && usage != VMA_MEMORY_USAGE_GPU_ONLY)
		{
			writeBuffer(bufferData);
			bufferData = nullptr;
		}
	}

	Buffer::Buffer(bs::Device* dev, const BufferUsage buf, const u64 size, const u64 stride, const void* data, VmaMemoryUsage usage)
		: p_device(dev), size(size), stride(stride), bufferType(buf), usage(usage)
	{
		m_buffer = VK_NULL_HANDLE;
		m_allocation = VK_NULL_HANDLE;

		memcpy(&bufferData, &data, sizeof(bufferData));

		if(this->stride == 0)
		{
			this->stride = 1;
		}

		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = usage;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = getSize();
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// To get the buffer type
		if (bufferType & bs::vk::BufferUsage::VERTEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::INDEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::UNIFORM_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::STORAGE_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::INDIRECT_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::TRANSFER_BUFFER)
		{
			if(usage == VMA_MEMORY_USAGE_GPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
			else if(usage == VMA_MEMORY_USAGE_CPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			}
			else
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
		}
		
		vmaCreateBuffer(p_device->getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

		if((bufferData != nullptr) && (usage != VMA_MEMORY_USAGE_GPU_ONLY))
		{
			writeBuffer(bufferData);
			bufferData = nullptr;
		}
	}

	Buffer::~Buffer()
	{
		deleteBuffer();
	}

	u64 Buffer::getStride() const
	{
		return stride;
	}

	u64 Buffer::getSize() const
	{
		return size;
	}

	u64 Buffer::getNumElements() const
	{
		return size / stride;
	}

	void Buffer::setMaxElements(u64 numElements)
	{
		setAllocationSize(stride * numElements);
	}

	void Buffer::setAllocationSize(const u64 numBytes)
	{
		size = numBytes;
	}
	
	void Buffer::allocateBuffer()
	{
		VmaAllocationCreateInfo allocInfo = {};
		allocInfo.usage = usage;

		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = getSize();
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		// To get the buffer type
		if (bufferType & bs::vk::BufferUsage::VERTEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::INDEX_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::UNIFORM_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::STORAGE_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::INDIRECT_BUFFER)
		{
			bufferInfo.usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
		}
		if (bufferType & bs::vk::BufferUsage::TRANSFER_BUFFER)
		{
			if(usage == VMA_MEMORY_USAGE_GPU_ONLY)
			{
				bufferInfo.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			}
			else if(usage == VMA_MEMORY_USAGE_CPU_ONLY)
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
		vmaCreateBuffer(p_device->getAllocator(), &bufferInfo, &allocInfo, &m_buffer, &m_allocation, nullptr);

		if(bufferData != nullptr && usage != VMA_MEMORY_USAGE_GPU_ONLY)
		{
			writeBuffer(bufferData);
			bufferData = nullptr;
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
		vmaMapMemory(p_device->getAllocator(), m_allocation, &bufferdata);
		//Add the offset
		bufferdata = (u8*)bufferdata + offset;
		memcpy(bufferdata, data, size);

		//Unmaps the buffer
		vmaUnmapMemory(p_device->getAllocator(), m_allocation);
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
		vmaDestroyBuffer(p_device->getAllocator(), m_buffer, m_allocation);
		m_buffer = VK_NULL_HANDLE;
		m_allocation = VK_NULL_HANDLE;
	}
}