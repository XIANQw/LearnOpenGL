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

    myLight::DirLight dirLight;
    const int N_POINTLIGHTS = 4;
    glm::vec3 pointLightPositions[] = {
        glm::vec3(0.7f,  0.2f,  2.0f),
        glm::vec3(2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3(0.0f,  0.0f, -3.0f)
    };
    std::vector<myLight::PointLight*> pointLights;
    for (int i = 0; i < N_POINTLIGHTS; i++) {
        myLight::PointLight* light = new myLight::PointLight(pointLightPositions[i]);
        light->p_obj = std::make_unique<GameObj>(Cube::makeCube());
        pointLights.push_back(light);
    }
    myLight::SpotLight spotLight;
    
    
    std::vector<const char*> files{ "../img/container2.png", "../img/container2_specular.png" };
    std::vector<int> wrapModes{ GL_REPEAT, GL_REPEAT };
    std::vector<int> filteringModes{ GL_LINEAR, GL_LINEAR };
    TextureSetter textureSetter(files, wrapModes, filteringModes);
    auto textures = textureSetter.getTextures();
    cube.textures = textures;

    Shader shader("3.3.shader.vert", "3.3.shader.frag");
    shader.use();
    shader.setVec3("dirLight.ambient", dirLight.ambient);
    shader.setVec3("dirLight.diffuse", dirLight.diffuse);
    shader.setVec3("dirLight.specular", dirLight.specular);
    shader.setVec3("dirLight.Dir", dirLight.direction);

    pointLights[0]->color = glm::vec3(1.0f, 0.5f, 0.3f);
    pointLights[1]->color = glm::vec3(0.5f, 1.0f, 0.3f);
    pointLights[2]->color = glm::vec3(0.3f, 0.5f, 1.0f);
    pointLights[3]->color = glm::vec3(0.3f, 0.5f, 0.3f);

    for (int i = 0; i < N_POINTLIGHTS; i++) {
        std::string str_i = std::to_string(i);
        shader.setVec3("pointLights[" + str_i + "].ambient", pointLights[i]->ambient);
        shader.setVec3("pointLights[" + str_i + "].diffuse", pointLights[i]->diffuse);
        shader.setVec3("pointLights[" + str_i + "].specular", pointLights[i]->specular);

        shader.setFloat("pointLights[" + str_i + "].Kc", pointLights[i]->Kc);
        shader.setFloat("pointLights[" + str_i + "].Kl", pointLights[i]->Kl);
        shader.setFloat("pointLights[" + str_i + "].Kq", pointLights[i]->Kq);

        shader.setVec3("pointLights[" + str_i + "].Pos", pointLights[i]->position);
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
    shader.setFloat("spotLight.Kq", spotLight.Kq);

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
        GLuint elapsed_time;
        UINT32 query;
        glGenQueries(1, &query);
        glBeginQuery(GL_TIME_ELAPSED, query);


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
        for (int i = 0; i < N_POINTLIGHTS; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLights[i]->position);
            model = glm::scale(model, glm::vec3(0.1f));
            lightShader.setMat4("model", model);
            lightShader.setVec3("lightColor", pointLights[i]->color);
            pointLights[i]->p_obj->draw();
        }

        shader.use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setVec3("cameraPos", camera.cameraPos);
        shader.setBool("myModel", myModel);
        shader.setVec3("spotLight.Dir", camera.cameraFront);
        shader.setVec3("spotLight.Pos", camera.cameraPos);
        for (int i = 0; i < N_POINTLIGHTS; i++) {
            std::string str_i = std::to_string(i);
            shader.setVec3("pointLights[" + str_i + "].Color", pointLights[i]->color);
        }

        cube.bindTexturesToGL();
        int scale = 1;
        for (int i = 0; i < cubePositions.size()*scale; i++) {
            int index = i % cubePositions.size();
            model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[index]);
            model = glm::rotate(model, (float)glfwGetTime() * glm::radians(10.0f * (index+1)), glm::vec3(0.5f, 1.0f, 0.0f));
            shader.setMat4("model", model);
            cube.draw();
        }

        glEndQuery(GL_TIME_ELAPSED);
        GLint getResult = 0;
        while (!getResult) {
            glGetQueryObjectiv(query, GL_QUERY_RESULT_AVAILABLE, &getResult);
        }
        glGetQueryObjectuiv(query, GL_QUERY_RESULT, &elapsed_time);
        std::cout << "Time spent on GPU:" << elapsed_time / 1000000.0 << std::endl;

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwTerminate();
    return 0;
}