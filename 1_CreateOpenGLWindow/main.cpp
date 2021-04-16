#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Windows.h>

#include "OpenGLWindow.h"
#include "Triangle.h"
#include "Shader.h"
#include "TextureSetter.h"
#include "GameObj.h"

float alpha = 0.2f;

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        alpha = std::min(1.0f, alpha + 0.0001f);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        alpha = std::max(0.0f, alpha - 0.0001f);
    }
}



int main()
{
    OpenGLWindow openglWindow(800, 600, "LearnOpenGL");
    auto window = openglWindow.getWindow();

    Shader shader("3.3.shader.vs", "3.3.shader.fs");
    //Triangle t = Triangle::defaultTriangle();
    //auto vertices = t.getVertexUVCoords();
    //auto indices = t.getIndices();

    std::vector<float> vertices{
        //     ---- 位置 ----          - 纹理坐标 -
             0.5f,  0.5f, 0.0f,    1.f, 1.f,   // 右上
             0.5f, -0.5f, 0.0f,    1.f, 0.f,   // 右下
            -0.5f, -0.5f, 0.0f,    0.f, 0.f,   // 左下
            -0.5f,  0.5f, 0.0f,    0.f, 1.f    // 左上
    };
    std::vector<uint32_t> vertexAttrLayout{ 0, 3, 5 };

    std::vector<uint32_t> indices{
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    GameObj obj(vertices, vertexAttrLayout, indices);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    std::vector<const char*> files{ "img/container.jpg", "img/awesomeface.png"};
    std::vector<int> formats{ GL_RGB, GL_RGBA };
    std::vector<int> wrapModes{ GL_REPEAT, GL_MIRRORED_REPEAT };
    std::vector<int> filteringModes{ GL_LINEAR, GL_LINEAR };
    TextureSetter textureSetter(files, formats, wrapModes, filteringModes);
    auto texture = textureSetter.getTextures();
    obj.setTextures(texture);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));
    trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

    shader.use();
    shader.setInt("texture0", 0);
    shader.setInt("texture1", 1);

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        obj.bindTexturesToGL();

        shader.use();
        shader.setFloat("alpha", alpha);

        obj.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}