#pragma once

#include "Buffer.h"

namespace bs::vk
{
	// This is a renderable 3D mesh
	class Model
	{
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
			vertexBuffer.size = mesh.vertices.size() * sizeof(bs::Vertex);
			vertexBuffer.stride = sizeof(bs::Vertex);
			vertexBuffer.bufferData = mesh.vertices.data();

			m_vertbuffer = std::make_shared<Buffer>(vertexBuffer);

			BufferDescription indexBuffer{};
			indexBuffer.bufferType = bs::vk::BufferUsage::INDEX_BUFFER;
			indexBuffer.dev = dev;
			indexBuffer.size = mesh.indicies.size() * sizeof(u32);;
			indexBuffer.stride = sizeof(u32);
			indexBuffer.bufferData = mesh.indicies.data();

			m_indexbuffer = std::make_shared<Buffer>(indexBuffer);
		}
		/*//Upload Mesh buffers to the GPU
		void uploadMesh()
		{
			for (auto& buffer : m_modelbuffers)
			{
				buffer.get()->allocateBuffer();
			}
		}*/

		bs::vk::Buffer* getVertexBuffer()
		{
			return m_vertbuffer.get();
		}

		bs::vk::Buffer* getIndexBuffer()
		{
			return m_indexbuffer.get();;
		}
	private:
		std::shared_ptr<Buffer> m_vertbuffer;
		std::shared_ptr<Buffer> m_indexbuffer;
	};
}