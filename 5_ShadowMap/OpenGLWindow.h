#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// Callback function : It will reset viewport while screen size is changed
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

class OpenGLWindow {
private:
	GLFWwindow* window;
public:
	float width, height;

	OpenGLWindow(uint32_t width, uint32_t height, const char* title) :width(width), height(height) {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(width, height, title, NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
		}
		glfwMakeContextCurrent(window);
		// Load liberaiy glad
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
		}
		// Set callback function : It will be called while screen size is changed
		glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
		glViewport(0, 0, width, height);
	}

	GLFWwindow* getWindow() {
		return window;
	}

	void bindToRenderTarget() {
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0, 0, width, height);
	}

};