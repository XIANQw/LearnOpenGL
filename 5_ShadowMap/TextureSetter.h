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
	t_normalmap
};

struct Texture {
	uint32_t id;
	texture_type type;
	std::string path;
};



class TextureSetter {
private:
	std::vector<int> widths, heights, nrComponents;
	unsigned char* data = nullptr;
	std::vector<Texture> textures;
public:
	TextureSetter(const std::vector<const char*>& filenames,
		const std::vector<texture_type> types,
		const std::vector<int>& wrapMode,
		const std::vector<int>& filteringMode) {
		auto nbTexture = filenames.size();
		if (wrapMode.size() < nbTexture || filteringMode.size() < nbTexture) {
			std::cout << "TextureSetter error" << std::endl;
			exit(-1);
		}
		stbi_set_flip_vertically_on_load(true);
		textures.resize(nbTexture);
		widths.resize(nbTexture);
		heights.resize(nbTexture);
		nrComponents.resize(nbTexture);
		for (int i = 0; i < nbTexture; i++) {
			textures[i].type = types[i];
			// Generate a texture and set id to 'texture's adresse
			glGenTextures(1, &textures[i].id);
			// Binding texture 2D
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, textures[i].id);
			// Set wrap and filter attributs
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode[i]); // S and T direction's wrap mode: repeat
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode[i]);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMode[i]); // Min and Mag filter mode : linear
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMode[i]);
			// Load data from image
			data = stbi_load(filenames[i], widths.data() + i, heights.data()+i, nrComponents.data() + i, 0);
			if (data) {
				GLenum format = GL_RGB;
				if (nrComponents[i] == 1)
					format = GL_RED;
				else if (nrComponents[i] == 3)
					format = GL_RGB;
				else if (nrComponents[i] == 4)
					format = GL_RGBA;
				// Copy image data to texture2D
				glTexImage2D(GL_TEXTURE_2D, 0, format, widths[i], heights[i], 0, format, GL_UNSIGNED_BYTE, data);
				// Generate Mipmap
				glGenerateMipmap(GL_TEXTURE_2D);
			}
			else {
				std::cout << "Texture " << i << " : Failed to load texture" << std::endl;
			}
			// Free data
			stbi_image_free(data);
		}

	}
	std::vector<Texture> getTextures() {
		return textures;
	}
};