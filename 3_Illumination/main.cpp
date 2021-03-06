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
bool myModel = true, vShaderLight = false;
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
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        myModel = true;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        myModel = false;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        vShaderLight = true;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        vShaderLight = false;
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

    glm::vec3 objColor = glm::vec3(1.0f, 0.5f, 0.31f);
    glm::vec3 lightColor = glm::vec3(1.0f);

    GameObj cube = GameObj::genCubeWithPosNormal();
    cube.material.shininess = 256;
    cube.material.objColorRGBA = glm::vec4(objColor, 1.0f);

    GameObj light = GameObj::genCube();
    light.material.objColorRGBA = glm::vec4(lightColor, 1.0f);

    Shader shader("3.3.shader.vert", "3.3.shader.frag");
    Shader lightShader("lightShader.vert", "lightShader.frag");
    shader.use();
    shader.setVec3("material.Ka", cube.material.Ka);
    shader.setVec3("material.Kd", cube.material.Kd);
    shader.setVec3("material.Ks", cube.material.Ks);
    shader.setFloat("material.shininess", cube.material.shininess);
    shader.setVec3("objColor", cube.material.objColorRGBA.x, cube.material.objColorRGBA.y, cube.material.objColorRGBA.z);


    std::vector<glm::vec3> cubePositions{
        glm::vec3(-1.0f,  -1.5f, -0.75f),
    };
    glm::vec3 lightPos = glm::vec3(cubePositions[0].x + 0.8, cubePositions[0].y + 1.0f, cubePositions[0].z);


    glEnable(GL_DEPTH_TEST);
    
    float deltaTime = 0.0f; // ??????????????????????
    float lastFrame = 0.0f; // ????????????

    // Render loop
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        camera.setCameraSpeed(2.5f * deltaTime);

        processInput(window);


        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        
        /*lightPos = glm::vec3(cubePositions[0].x + 0.8 * sin((float)glfwGetTime()), cubePositions[0].y + 1.0f, cubePositions[0].z + sin((float)glfwGetTime()));*/
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        view = camera.getLookAt();
        projection = glm::perspective(glm::radians(camera.getZoom()), openglWindow.width / openglWindow.height, 0.1f, 100.0f);

        light.material.objColorRGBA.x = sin(glfwGetTime() * 2.0f);
        light.material.objColorRGBA.y = sin(glfwGetTime() * 0.7f);
        light.material.objColorRGBA.z = sin(glfwGetTime() * 1.3f);
        lightShader.use();
        lightShader.setMat4("model", model);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", light.material.objColorRGBA.x, light.material.objColorRGBA.y, light.material.objColorRGBA.z);
        light.draw();
        cube.bindTexturesToGL();
        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.cameraPos);
        shader.setVec3("lightColor", light.material.objColorRGBA.x, light.material.objColorRGBA.y, light.material.objColorRGBA.z);
        shader.setVec3("lightPos", lightPos);
        shader.setBool("myModel", myModel);
        shader.setBool("vShaderLight", vShaderLight);
        for (int i = 0; i < cubePositions.size(); i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            //model = glm::rotate(model, (float)glfwGetTime() * glm::radians(10.0f * (i+1)), glm::vec3(0.5f, 1.0f, 0.0f));
            shader.setMat4("model", model);
            cube.draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}