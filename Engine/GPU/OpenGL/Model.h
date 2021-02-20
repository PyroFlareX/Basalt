#pragma once

#include "../Engine/Resources/Mesh.h"
#include "../Engine/glad/glad/glad.h"

class Model
{
public:
	Model() = default;
	Model(const vn::Mesh& mesh)
	{
		addData(mesh);
	}

	~Model() { deleteData(); }

	void addData(const vn::Mesh& mesh)
	{
		genVAO();

		//addVBO(3, mesh.vertices);
		//addVBO(2, mesh.texCoords);

		addVBOVert(3, mesh.vertices);
		addEBO(mesh.indicies);

		glBindVertexArray(0);
	}

	void deleteData()
	{
		glBindVertexArray(0);
		glDeleteVertexArrays(1, &m_info.VAO);
		
		if (bufferObjects.size() > 0)
		{
			glDeleteBuffers(bufferObjects.size(), bufferObjects.data());
		}

		bufferObjects.clear();
		vboCount = 0;
	}

	void genVAO()
	{
		glGenVertexArrays(1, &m_info.VAO); //VAO
		glBindVertexArray(m_info.VAO);		//Bind VAO
	}
	void bindVAO()
	{
		glBindVertexArray(m_info.VAO);		//Bind VAO
	}

	void addVBO(int dim, const std::vector<float>& data)
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);		//Gen VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(/*vn::Vertex*/ float) * data.size(), data.data(), GL_STATIC_DRAW);	//Store Verticies
		//Shader Attributes
		//Position
		glVertexAttribPointer(static_cast<GLuint>(vboCount), dim, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(static_cast<GLuint>(vboCount++));
		//Normals
		/*glVertexAttribPointer(static_cast<GLuint>(vboCount), dim, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(static_cast<GLuint>(vboCount++));
		//UV
		glVertexAttribPointer(static_cast<GLuint>(vboCount), dim, GL_FLOAT, GL_FALSE, 0, nullptr);
		glEnableVertexAttribArray(static_cast<GLuint>(vboCount++));*/

		bufferObjects.push_back(VBO);
	}

	void addVBOVert(int dim, const std::vector<vn::Vertex>& data)
	{
		unsigned int VBO;
		glGenBuffers(1, &VBO);		//Gen VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vn::Vertex) * data.size(), data.data(), GL_STATIC_DRAW);	//Store Verticies
		//Shader Attributes
		//Position
		glEnableVertexAttribArray(static_cast<GLuint>(vboCount));
		glVertexAttribPointer(static_cast<GLuint>(vboCount), 3, GL_FLOAT, GL_FALSE, sizeof(vn::Vertex), nullptr);
		
		//Normals
		glEnableVertexAttribArray(static_cast<GLuint>(++vboCount));
		glVertexAttribPointer(static_cast<GLuint>(vboCount), 3, GL_FLOAT, GL_FALSE, sizeof(vn::Vertex), (void*)(3 * sizeof(float)));
		
		//UV
		glEnableVertexAttribArray(static_cast<GLuint>(++vboCount));
		glVertexAttribPointer(static_cast<GLuint>(vboCount), 2, GL_FLOAT, GL_FALSE, sizeof(vn::Vertex), (void*)(6 * sizeof(float)));
		

		bufferObjects.push_back(VBO);
	}

	void addEBO(const std::vector<unsigned int>& indicies)
	{
		m_info.indiciesCount = indicies.size();
		unsigned int EBO;
		glGenBuffers(1, &EBO);		//Gen EBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indicies.size(), indicies.data(), GL_STATIC_DRAW);	//Store Indicies
	}

	//Unnecessary Currently
	/*void addSSBO(const std::vector<int>& data)
	{
		unsigned int SSBO;
		glGenBuffers(1, &SSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), data.data(), /*?*//*);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, SSBO);
		bufferObjects.push_back(SSBO);
	}

	void addBuffer(GLenum type, const std::vector<int>& data)
	{
		unsigned int BO;
		glGenBuffers(1, &BO);
		glBindBuffer(type, BO);
		bufferObjects.push_back(BO);
	}*/

	int getNumIndicies() const { return m_info.indiciesCount; }

	vn::renderInfo& getInfo() { return m_info; }

private:
	vn::renderInfo m_info;
	int vboCount = 0;
	std::vector<unsigned int> bufferObjects;

};