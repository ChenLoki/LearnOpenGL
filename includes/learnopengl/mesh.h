#pragma once
#include <glad/glad.h> // 所有头文件
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>

using namespace std;


struct Vertex
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
	glm::vec3 Bitangent;
};

struct Texture
{
	unsigned int id;
	string type;
	string path;
};

class Mesh
{
public:
	vector<Vertex>       vertices;
	vector<unsigned int> indices;
	vector<Texture>      textures;
	unsigned int VAO;// 一个mesh的VAO是唯一的

	Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures)
	{
		this->vertices = vertices;
		this->indices  = indices;
		this->textures = textures;
        
        // 构建VAO
        setupMesh();
	}

    // 进入Draw之前已经绑定了shader
	void Draw(Shader shader)
	{
		unsigned int diffuseNr  = 1;
		unsigned int specularNr = 1;
		unsigned int normalNr   = 1;
		unsigned int heightNr   = 1;
        
        // 绑定texture
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			glActiveTexture(GL_TEXTURE0 + i);
											  
			string number;
			string name = textures[i].type;
			if      (name == "texture_diffuse") number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")number = std::to_string(specularNr++);
            else if (name == "texture_normal")  number = std::to_string(normalNr++);
            else if (name == "texture_height")  number = std::to_string(heightNr++);
            
			glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);// 设置纹理的绑定槽
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        
        // 绘制结束，结束VAO的绑定
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
	}

private:
	unsigned int VBO, EBO;

	void setupMesh()
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
        {
            // 对应drawArray
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
            
            // 对应drawElement
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
            
            // 开启shader中的location锚点，这些数据存在于VBO中
            glEnableVertexAttribArray(0);glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Position));
            glEnableVertexAttribArray(1);glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
            glEnableVertexAttribArray(2);glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
            glEnableVertexAttribArray(3);glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
            glEnableVertexAttribArray(4);glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        }
		glBindVertexArray(0);
	}
};

