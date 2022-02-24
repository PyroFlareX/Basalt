#pragma once

#include "Buffer.h"

namespace bs::vk
{
	// This is a renderable 3D mesh
	class Model
	{
		std::shared_ptr<Buffer> m_vertbuffer;
		std::shared_ptr<Buffer> m_indexbuffer;
	public:
		Model(const bs::Mesh& mesh, bs::Device* dev)
		{
			addData(mesh, dev);
		}
		~Model() = default;

		//Buffer the mesh
		void addData(const bs::Mesh& mesh, bs::Device* dev)
		{
			BufferDescription vertexBuffer{};
			vertexBuffer.bufferType = bs::vk::BufferUsage::VERTEX_BUFFER;
			vertexBuffer.dev = dev;
			vertexBuffer.size = mesh.vertices.size() * sizeof(mesh.vertices[0]);
			vertexBuffer.stride = sizeof(mesh.vertices[0]);
			vertexBuffer.bufferData = const_cast<void*>((void*)mesh.vertices.data());

			m_vertbuffer = std::make_shared<Buffer>(vertexBuffer);

			BufferDescription indexBuffer{};
			indexBuffer.bufferType = BufferUsage::INDEX_BUFFER;
			indexBuffer.dev = dev;
			indexBuffer.size = mesh.indicies.size() * sizeof(mesh.indicies[0]);
			indexBuffer.stride = sizeof(mesh.indicies[0]);
			indexBuffer.bufferData = const_cast<void*>((void*)mesh.indicies.data());

			m_indexbuffer = std::make_shared<Buffer>(indexBuffer);
		}

		Buffer* getVertexBuffer() const
		{
			return m_vertbuffer.get();
		}

		Buffer* getIndexBuffer() const
		{
			return m_indexbuffer.get();;
		}
	};
}