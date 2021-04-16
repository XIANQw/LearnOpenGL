#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Windows.h>

#include "OpenGLWindow.h"
#include "Triangle.h"
#include "Shader.h"
#include "TextureSetter.h"
#include "GameObj.h"
#include "Camera.h"


const float WIDTH = 800, HEIGHT = 600;

Camera camera;

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.move(FORWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.move(BACKWARD);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.move(LEFT);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.move(RIGHT);
    }
    
}

bool firstMouse = true;
float lastX = WIDTH / 2, lastY = HEIGHT / 2;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    // reversed since y-coordinates go from bottom to top
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.updateYawPitch(xoffset, yoffset);
}


void scoll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.updateZoom(yoffset);
}


int main()
{
    OpenGLWindow openglWindow(WIDTH, HEIGHT, "LearnOpenGL");
    auto window = openglWindow.getWindow();
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scoll_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    Shader shader("3.3.shader.vs", "3.3.shader.fs");
    GameObj cube = GameObj::genCube();

    std::vector<glm::vec3> cubePositions{
        glm::vec3(0.0f,  0.0f,  0.0f),
        glm::vec3(2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3(2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3(1.3f, -2.0f, -2.5f),
        glm::vec3(1.5f,  2.0f, -2.5f),
        glm::vec3(1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    std::vector<const char*> files{ "img/container.jpg", "img/awesomeface.png"};
    std::vector<int> formats{ GL_RGB, GL_RGBA };
    std::vector<int> wrapModes{ GL_REPEAT, GL_REPEAT };
    std::vector<int> filteringModes{ GL_LINEAR, GL_LINEAR };
    TextureSetter textureSetter(files, formats, wrapModes, filteringModes);
    auto texture = textureSetter.getTextures();
    cube.setTextures(texture);


    shader.use();
    shader.setInt("texture0", 0);
    shader.setInt("texture1", 1);
    glEnable(GL_DEPTH_TEST);
    

    float deltaTime = 0.0f; // 当前帧与上一帧的时间差
    float lastFrame = 0.0f; // 上一帧的时间

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        camera.setCameraSpeed(2.5f * deltaTime);

        processInput(window);


        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube.bindTexturesToGL();

        glm::mat4 view = glm::mat4(1.0f);
        // 注意，我们将矩阵向我们要进行移动场景的反方向移动。
        view = camera.getLookAt();
        glm::mat4 projection = glm::mat4(1.0f);
        projection = glm::perspective(glm::radians(camera.getZoom()), openglWindow.width / openglWindow.height, 0.1f, 100.0f);

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);

        for (int i = 0; i < cubePositions.size(); i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(10.0f * (i+1)), glm::vec3(0.5f, 1.0f, 0.0f));
            shader.setMat4("model", model);
            cube.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}