#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>
class GameObj {
private:
	std::vector<float> vertices;
	std::vector<uint32_t> vertexAttrLayout;
	std::vector<uint32_t> indices;
	uint32_t VAO, VBO, EBO;
	std::vector<uint32_t> textures;
public:
	GameObj() = default;
	GameObj(const std::vector<float>& vertices,
		const std::vector<uint32_t>& vertexAttrLayout,
		const std::vector<uint32_t>& indices) : 
		vertices(vertices),
		vertexAttrLayout(vertexAttrLayout),
		indices(indices){
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		// 1. Binding VAO
		glBindVertexArray(VAO);
		// 2. Copy vertex data to VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		// 2.5 Binding EBO
		if (indices.size() > 0) {
			glGenBuffers(1, &EBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(uint32_t), indices.data(), GL_STATIC_DRAW);
		}
		// 3. Setting vertex position attributs
		auto totalLength = vertexAttrLayout.back();
		for (int i = 1; i < vertexAttrLayout.size(); i++) {
			auto start = vertexAttrLayout[i - 1];
			auto end = vertexAttrLayout[i];
			glVertexAttribPointer(i - 1, end - start, GL_FLOAT, GL_FALSE, totalLength * sizeof(float), (void*)(start*sizeof(float)));
			glEnableVertexAttribArray(i - 1);

		}
	}

	void setTextures(const std::vector<uint32_t>& textures) {
		this->textures = textures;
	}

	void bindTexturesToGL() {
		for (int i = 0; i < textures.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i]);
		}
	}

	void draw() {
		glBindVertexArray(VAO);
		if (indices.size() == 0) {
			glDrawArrays(GL_TRIANGLES, 0, vertices.size());
		}
		else {
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	GameObj static genCube() {
		std::vector<float> vertices{
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};
		std::vector<uint32_t> vertexAttrLayout{ 0, 3, 5 };
		std::vector<uint32_t> indices{};

		GameObj obj(vertices, vertexAttrLayout, indices);
		return obj;
	}

};