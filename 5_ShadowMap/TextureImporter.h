#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

enum texture_type {
	t_diffusemap,
	t_specularmap,
	t_normalmap,
	t_depthmap
};

struct Texture {
	uint32_t id;
	texture_type type;
	std::string path;
	int width;
	int height;
};

namespace TextureImporter {

	Texture importTexture(const char* filename,
		texture_type type,
		const int wrapMode = GL_REPEAT,
		const int filteringMode = GL_LINEAR) {

		Texture texture;
		texture.type = type;
		texture.path = filename;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = nullptr;
		
		texture.type = type;
		// Generate a texture and set id to 'texture's adresse
		glGenTextures(1, &texture.id);
		// Binding texture 2D
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		// Set wrap and filter attributs
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode); // S and T direction's wrap mode: repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMode); // Min and Mag filter mode : linear
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMode);



		// Load data from image
		int nrComponent;
		data = stbi_load(filename, &texture.width, &texture.height, &nrComponent, 0);
		if (data) {
			GLenum format = GL_RGB;
			if (nrComponent == 1)
				format = GL_RED;
			else if (nrComponent == 3)
				format = GL_RGB;
			else if (nrComponent == 4)
				format = GL_RGBA;
			// Copy image data to texture2D
			glTexImage2D(GL_TEXTURE_2D, 0, format, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, data);
			// Generate Mipmap
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Texture " << filename << " : Failed to load texture" << std::endl;
		}
		// Free data
		stbi_image_free(data);

		return texture;
	}

};