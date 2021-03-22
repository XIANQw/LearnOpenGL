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
#include "Light.h"

const float WIDTH = 800, HEIGHT = 600;

Camera camera;
bool myModel = true;
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

    Cube cube = Cube::makeCubeWithPosNormalTexcoords();
    cube.material.shininess = 256;

    myLight::PointLight light(glm::vec3(1.0f, 1.0f, 0));
    light.p_obj = std::make_unique<GameObj>(Cube::makeCube());
    
    std::vector<const char*> files{ "img/container2.png", "img/container2_specular.png" };
    std::vector<int> wrapModes{ GL_REPEAT, GL_REPEAT };
    std::vector<int> filteringModes{ GL_LINEAR, GL_LINEAR };
    TextureSetter textureSetter(files, wrapModes, filteringModes);
    auto textures = textureSetter.getTextures();
    cube.textures = textures;

    Shader shader("3.3.shader.vert", "3.3.shader.frag");
    shader.use();
    //shader.setVec3("pointLight.ambient", light.ambient);
    //shader.setVec3("pointLight.diffuse", light.diffuse);
    //shader.setVec3("pointLight.specular", light.specular);
    //shader.setFloat("pointLight.Kc", light.Kc);
    //shader.setFloat("pointLight.Kl", light.Kl);
    //shader.setFloat("pointLight.Kq", light.Kq);
    //shader.setVec3("pointLight.Pos", light.position);
    //shader.setVec3("pointLight.Color", light.color);

    shader.setVec3("spotLight.ambient", light.ambient);
    shader.setVec3("spotLight.diffuse", light.diffuse);
    shader.setVec3("spotLight.specular", light.specular);
    shader.setVec3("spotLight.Color", light.color);
    shader.setFloat("spotLight.Kc", light.Kc);
    shader.setFloat("spotLight.Kl", light.Kl);
    shader.setFloat("spotLight.Kq", light.Kq);
    shader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));


    shader.setFloat("material.shininess", cube.material.shininess);
    shader.setInt("material.diffuseMap", 0);
    shader.setInt("material.specularMap", 1);

    Shader lightShader("lightShader.vert", "lightShader.frag");


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

        glClearColor(0.1, 0.1, 0.1, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        
        light.position = glm::vec3(cubePositions[0].x + 0.8, cubePositions[0].y + 1.0f, cubePositions[0].z);
        model = glm::translate(model, light.position);
        model = glm::scale(model, glm::vec3(0.1f));
        view = camera.getLookAt();
        projection = glm::perspective(glm::radians(camera.getZoom()), openglWindow.width / openglWindow.height, 0.1f, 100.0f);

        //lightShader.use();
        //lightShader.setMat4("model", model);
        //lightShader.setMat4("view", view);
        //lightShader.setMat4("projection", projection);
        //lightShader.setVec3("lightColor", light.color);

        //light.p_obj->draw();

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.cameraPos);
        shader.setBool("myModel", myModel);
        shader.setVec3("spotLight.Dir", camera.cameraFront);
        shader.setVec3("spotLight.Pos", camera.cameraPos);


        cube.bindTexturesToGL();
        for (int i = 0; i < cubePositions.size(); i++) {
            model = glm::mat4(1.0f);
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