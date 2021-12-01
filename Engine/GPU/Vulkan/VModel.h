#pragma once

#include "Buffer.h"

namespace bs::vk
{
	// This is a renderable 3D mesh
	class Model
	{
	public:
		Model(const bs::Mesh& mesh, bs::Device* dev)	:	m_mesh(mesh)
		{
			addData(m_mesh, dev);
		}
		//Buffer the mesh
		void addData(bs::Mesh& mesh, bs::Device* dev)
		{
			BufferDescription vertexBuffer{};
			vertexBuffer.bufferType = bs::vk::BufferUsage::VERTEX_BUFFER;
			vertexBuffer.dev = dev;
			vertexBuffer.size = m_mesh.vertices.size() * sizeof(bs::Vertex);
			vertexBuffer.stride = sizeof(bs::Vertex);
			vertexBuffer.bufferData = m_mesh.vertices.data();

			BufferDescription indexBuffer{};
			indexBuffer.bufferType = bs::vk::BufferUsage::INDEX_BUFFER;
			indexBuffer.dev = dev;
			indexBuffer.size = m_mesh.indicies.size() * sizeof(unsigned int);;
			indexBuffer.stride = sizeof(unsigned int);
			indexBuffer.bufferData = m_mesh.indicies.data();
			
			m_modelbuffers.emplace_back(std::make_shared<Buffer>(vertexBuffer));
			m_modelbuffers.emplace_back(std::make_shared<Buffer>(indexBuffer));

			uploadMesh();
		}
		//Upload Mesh buffers to the GPU
		void uploadMesh()
		{
			for (auto& buffer : m_modelbuffers)
			{
				buffer.get()->uploadBuffer();
			}
		}

		bs::vk::Buffer* getVertexBuffer()
		{
			
			return m_modelbuffers.at(0).get();
		}

		bs::vk::Buffer* getIndexBuffer()
		{
			return m_modelbuffers.at(1).get();
		}

		~Model() = default;

	private:
		std::vector<std::shared_ptr<Buffer>> m_modelbuffers;
		bs::Mesh m_mesh;
	};
}