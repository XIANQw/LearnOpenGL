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

	Texture newDepthMap(int width, int height) {
		// Create a depth texutre
		Texture depthmap;
		depthmap.width = width;
		depthmap.height = height;
		depthmap.type = t_depthmap;
		glGenTextures(1, &depthmap.id);
		glBindTexture(GL_TEXTURE_2D, depthmap.id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		return depthmap;
	}

	Texture newTexture(int width, int height, texture_type type) {
		Texture tex;
		tex.width = width;
		tex.height = height;
		tex.type = type;
		glGenTextures(1, &tex.id);
		glBindTexture(GL_TEXTURE_2D, tex.id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
			width, height, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		return tex;
	}

	uint32_t bindDepthMapToFBO(uint32_t depthMapID) {
		// Create a FBO for our depthmap
		GLuint depthMapFBO;
		glGenFramebuffers(1, &depthMapFBO);
		// Bind FB to pipeline and bind depth map to FB
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMapID, 0);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return depthMapFBO;
	}

	uint32_t bindFBO(uint32_t texID) {
		// Create a FBO for our depthmap
		GLuint myFBO;
		glGenFramebuffers(1, &myFBO);
		// Bind FB to pipeline and bind depth map to FB
		glBindFramebuffer(GL_FRAMEBUFFER, myFBO);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texID, 0);
		return myFBO;
	}

};