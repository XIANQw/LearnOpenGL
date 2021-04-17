#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <Windows.h>

#include "OpenGLWindow.h"
#include "Triangle.h"
#include "Shader.h"
#include "TextureImporter.h"
#include "Mesh.h"
#include "Camera.h"
#include "Light.h"
#include "Shape.h"
#include "Model.h"

const float WIDTH = 800, HEIGHT = 600;

Camera camera;
bool compare = false;
float deltaY = 0.0f;
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
        compare = true;
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        compare = false;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        deltaY += 0.01;
        std::cout << deltaY << std::endl;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        deltaY -= 0.01;
        std::cout << deltaY << std::endl;
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

    auto floor = Shape::makePlane();
    Texture floorTexture = TextureImporter::importTexture("../img/floor.png", t_diffusemap);
    floor.textures.push_back(floorTexture);

    Model gameObj("../img/nanosuit/nanosuit.obj");

    /* 为了做shadow mapping, 需要将camera放在光源位置
       所以这里用 SpotLight 充当 Directional Light
    */
    myLight::SpotLight dirLight;
    dirLight.position = glm::vec3(-3, 3, 3);
    dirLight.direction = glm::vec3(0) - dirLight.position;
    dirLight.p_obj = std::make_shared<Mesh<Vertex>>(Shape::makeCube());
    //std::vector<glm::vec3> pointLightPositions = {
    //    //glm::vec3(-0.3f,  -1.0f, -0.3f),
    //    //glm::vec3(0.3f,  0.5f, -0.3f),
    //    //glm::vec3(0.3f,  1.0f,  0.3f),
    //    glm::vec3(-0.5f, 0.5f, 0.5f)
    //};
    //const int N_POINTLIGHTS = pointLightPositions.size();
    //std::vector<myLight::PointLight*> pointLights;
    //for (int i = 0; i < N_POINTLIGHTS; i++) {
    //    myLight::PointLight* light = myLight::newPointLight(pointLightPositions[i]);
    //    pointLights.push_back(light);
    //}
    //myLight::SpotLight spotLight;

   
    Shader shader("3.3.shader.vert", "3.3.shader.frag");
    shader.use();
    shader.setVec3("dirLight.ambient", dirLight.ambient);
    shader.setVec3("dirLight.diffuse", dirLight.diffuse);
    shader.setVec3("dirLight.specular", dirLight.specular);
    shader.setVec3("dirLight.Dir", dirLight.direction);
    shader.setVec3("dirLight.Color", dirLight.color);


    /* Set all pointLight and spotLight info
    for (int i = 0; i < N_POINTLIGHTS; i++) {
        std::string str_i = std::to_string(i);
        shader.setVec3("pointLights[" + str_i + "].ambient", pointLights[i]->ambient);
        shader.setVec3("pointLights[" + str_i + "].diffuse", pointLights[i]->diffuse);
        shader.setVec3("pointLights[" + str_i + "].specular", pointLights[i]->specular);

        shader.setFloat("pointLights[" + str_i + "].Kc", pointLights[i]->Kc);
        shader.setFloat("pointLights[" + str_i + "].Kl", pointLights[i]->Kl);
        shader.setFloat("pointLights[" + str_i + "].Kq", pointLights[i]->Kq);
        shader.setVec3("pointLights[" + str_i + "].Color", pointLights[i]->color);
    }

    shader.setVec3("spotLight.ambient", spotLight.ambient);
    shader.setVec3("spotLight.diffuse", spotLight.diffuse);
    shader.setVec3("spotLight.specular", spotLight.specular);

    shader.setVec3("spotLight.Color", spotLight.color);

    shader.setFloat("spotLight.cutOff", spotLight.cutOff);
    shader.setFloat("spotLight.outerCutOff", spotLight.outerCutOff);
    
    shader.setFloat("spotLight.Kc", spotLight.Kc);
    shader.setFloat("spotLight.Kl", spotLight.Kl);
    shader.setFloat("spotLight.Kq", spotLight.Kq);*/
    shader.setFloat("material.shininess", floor.material.shininess);


    Shader lightShader("lightShader.vert", "lightShader.frag");


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
        view = camera.getLookAt();
        projection = glm::perspective(glm::radians(camera.getZoom()), openglWindow.width / openglWindow.height, 0.1f, 100.0f);
        
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        /* 
            Draw dirLight
        */
        model = glm::mat4(1.0f);
        //model = glm::rotate(model, float(glfwGetTime() * glm::radians(60.0f)), glm::vec3(0,1,0));
        model = glm::translate(model, dirLight.position);
        model = glm::scale(model, glm::vec3(0.1f));
        lightShader.setMat4("model", model);
        lightShader.setVec3("lightColor", dirLight.color);
        dirLight.p_obj->draw(lightShader);
        

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.cameraPos);
        shader.setBool("compare", compare);
        /* Set pointLight real-time info
        shader.setVec3("spotLight.Dir", camera.cameraFront);
        shader.setVec3("spotLight.Pos", camera.cameraPos);
        for (size_t i = 0; i < N_POINTLIGHTS; i++) {
            std::string str_i = std::to_string(i);
            shader.setVec3("pointLights[" + str_i + "].Color", pointLights[i]->color);
            shader.setVec3("pointLights[" + str_i + "].Pos", pointLights[i]->position);
        }
        */

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0));
        shader.setMat4("model", model);
        floor.draw(shader);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0, -0.5, 0));
        model = glm::scale(model, glm::vec3(0.1f));
        shader.setMat4("model", model);    
        gameObj.Draw(shader);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}