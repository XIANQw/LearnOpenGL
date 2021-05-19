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
	uint32_t m_id;
	texture_type m_type;
	std::string m_path;
	int m_width;
	int m_height;
	uint32_t m_fbo = 0;
	uint32_t m_rbo = 0;
	GLenum m_textureTarget;
	
	Texture() = default;
	Texture(int width, int height, 
		GLenum textureTarget, 
		GLfloat filter, 
		GLenum format, 
		GLenum internalFormat, 
		bool clamp, 
		GLenum attachment, 
		texture_type texturetype,
		GLfloat* boarderColor = nullptr,
		unsigned char* data = nullptr) :
		m_width(width), m_height(height), m_textureTarget(textureTarget), m_type(texturetype)
	{
		InitTexture(textureTarget, filter, format, internalFormat, clamp, boarderColor, data);
		InitRenderTargets(attachment);
	}


	void InitTexture(GLenum textureTarget, GLfloat filter, GLenum format, GLenum internalFormat, bool clamp, GLfloat* boarderColor, unsigned char* data) {
		glGenTextures(1, &m_id);
		glBindTexture(textureTarget, m_id);
		
		glTexImage2D(textureTarget, 0, internalFormat, m_width, m_height, 0, format, GL_UNSIGNED_BYTE, data);

		glTexParameterf(textureTarget, GL_TEXTURE_MIN_FILTER, filter);
		glTexParameterf(textureTarget, GL_TEXTURE_MAG_FILTER, filter);
		if (clamp) {
			glTexParameterf(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameterf(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		}
		glTexParameteri(textureTarget, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(textureTarget, GL_TEXTURE_MAX_LEVEL, 0);
		if (boarderColor != nullptr) {
			glTexParameterfv(textureTarget, GL_TEXTURE_BORDER_COLOR, boarderColor);
		}
	}

	void InitRenderTargets(GLenum attachment) {
		bool hasDepth = false;
		GLenum drawBuffer = GL_NONE;
		if (attachment == GL_DEPTH_ATTACHMENT) {
			hasDepth = true;
		}
		else {
			drawBuffer = attachment;
		}
		if (m_fbo == 0) {
			glGenFramebuffers(1, &m_fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		}
		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, m_textureTarget, m_id, 0);

		if (m_fbo == 0) {
			return;
		}
		if (!hasDepth)
		{
			glGenRenderbuffers(1, &m_rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
			glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);
		}

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			std::cerr << "FBO is incomplete" << std::endl;
			assert(false);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void bindToRenderTarget() {
		glBindTexture(m_textureTarget, m_id);
		glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		glViewport(0, 0, m_width, m_height);
	}

	
};

namespace TextureImporter {

	Texture importTexture(const char* filename,
		texture_type type,
		const int wrapMode = GL_REPEAT,
		const int filteringMode = GL_LINEAR) {

		Texture texture;
		texture.m_type = type;
		texture.m_path = filename;
		stbi_set_flip_vertically_on_load(true);
		unsigned char* data = nullptr;
		
		texture.m_type = type;
		// Generate a texture and set id to 'texture's address
		glGenTextures(1, &texture.m_id);
		// Binding texture 2D
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture.m_id);
		// Set wrap and filter attributs
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode); // S and T direction's wrap mode: repeat
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filteringMode); // Min and Mag filter mode : linear
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filteringMode);

		// Load data from image
		int nrComponent;
		data = stbi_load(filename, &texture.m_width, &texture.m_height, &nrComponent, 0);
		if (data) {
			GLenum format = GL_RGB;
			if (nrComponent == 1)
				format = GL_RED;
			else if (nrComponent == 3)
				format = GL_RGB;
			else if (nrComponent == 4)
				format = GL_RGBA;
			// Copy image data to texture2D
			glTexImage2D(GL_TEXTURE_2D, 0, format, texture.m_width, texture.m_height, 0, format, GL_UNSIGNED_BYTE, data);
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