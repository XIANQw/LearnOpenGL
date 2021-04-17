#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
#include "Material.h"
#include "Vertex.h"
#include "Shader.h"
#include "TextureImporter.h"

template<class VertexType=VertexNormalTex>
class Mesh {

public:
	std::vector<VertexType> vertices;
	std::vector<uint32_t> indices;
	uint32_t VAO, VBO, EBO;
	myMaterial::Material material;
	std::vector<Texture> textures;
	std::shared_ptr<Texture> depthMap = nullptr;
	
	Mesh() = default;
	Mesh(const std::vector<VertexType>& vertices,
		const std::vector<uint32_t>& indices) : 
		vertices(vertices),
		indices(indices){
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// 1. Binding VAO
		glBindVertexArray(VAO);
		// 2. Copy vertex data to VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexType), vertices.data(), GL_STATIC_DRAW);
		// 2.5 Binding EBO
		if (indices.size() > 0) {
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		}
		// 3. Setting vertex position attributs
		auto totalLength = VertexType::layout.back();
		for (size_t i = 1; i < VertexType::layout.size(); i++) {
			auto start = VertexType::layout[i - 1];
			auto end = VertexType::layout[i];
			glVertexAttribPointer(i - 1, end - start, GL_FLOAT, GL_FALSE, totalLength * sizeof(float), (void*)(start*sizeof(float)));
			glEnableVertexAttribArray(i - 1);

		}
	}

	void bindTexturesToGL() {
		for (size_t i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
	}

	void draw(Shader shader) {
		shader.use();
		
		uint32_t diffuseMapN = 1;
		uint32_t specularMapN = 1;
		for (size_t i = 0; i < textures.size(); i++) {
			std::string number;
			glActiveTexture(GL_TEXTURE0 + i);
			if (textures[i].type == texture_type::t_diffusemap) {
				number = std::to_string(diffuseMapN++);
				shader.setInt("material.diffuseMap" + number, i);
			}
			else if (textures[i].type == texture_type::t_specularmap) {
				number = std::to_string(specularMapN++);
				shader.setInt("material.specularMap" + number, i);
			}
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
		}
		if (depthMap != nullptr) {
			glActiveTexture(GL_TEXTURE0 + textures.size());
			shader.setInt("depthMap", textures.size());
			glBindTexture(GL_TEXTURE_2D, depthMap->id);
		}
		
		glBindVertexArray(VAO);
		if (indices.size() == 0) {
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
		else {
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

};

