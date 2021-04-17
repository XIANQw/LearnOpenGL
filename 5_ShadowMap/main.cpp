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
bool controlBox = true;
float deltaY = 0.0f;
glm::vec3 currentBoxPos = camera.cameraPos;
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
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        controlBox = false;
        currentBoxPos = camera.cameraPos + camera.cameraFront * 2.0f;
        currentBoxPos.y = std::max(0.0f, currentBoxPos.y);
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        controlBox = true;
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


// renders the 3D scene
// --------------------
inline void renderScene(const Shader& shader, Mesh<VertexNormalTex>& floor, Model& gameObj, Mesh<VertexNormalTex>& box)
{
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0));
    shader.setMat4("model", model);
    shader.setBool("material.useSpecularMap", false);
    floor.draw(shader);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0, -0.5, 0));
    model = glm::scale(model, glm::vec3(0.1f));
    shader.setMat4("model", model);
    shader.setBool("material.useSpecularMap", true);
    gameObj.Draw(shader);

    model = glm::mat4(1.0f);
    if (controlBox) {
        currentBoxPos = camera.cameraPos + camera.cameraFront * 2.0f;
        currentBoxPos.y = std::max(0.0f, currentBoxPos.y);
    }
    model = glm::translate(model, currentBoxPos);
    
    shader.setMat4("model", model);
    box.draw(shader);
}

unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
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

    auto box = Shape::makeCubeWithPosNormalTexcoords();
    Texture boxDiffTex = TextureImporter::importTexture("../img/container2.png", t_diffusemap);
    Texture boxSpecTex = TextureImporter::importTexture("../img/container2_specular.png", t_specularmap);
    box.textures.push_back(boxDiffTex);
    box.textures.push_back(boxSpecTex);

    Model gameObj("../img/nanosuit/nanosuit.obj");

    /* 
        为了做shadow mapping, 需要将camera放在光源位置
        所以这里用 SpotLight 充当 Directional Light
    */
    myLight::SpotLight dirLight;
    dirLight.position = glm::vec3(-3, 3, 3);
    dirLight.direction = glm::vec3(0) - dirLight.position;
    dirLight.p_obj = std::make_shared<Mesh<Vertex>>(Shape::makeCube());
    Shader shader("3.3.shader.vert", "3.3.shader.frag");
    shader.use();
    shader.setVec3("dirLight.ambient", dirLight.ambient);
    shader.setVec3("dirLight.diffuse", dirLight.diffuse);
    shader.setVec3("dirLight.specular", dirLight.specular);
    shader.setVec3("dirLight.Dir", dirLight.direction);
    shader.setVec3("dirLight.Color", dirLight.color);
    shader.setFloat("material.shininess", floor.material.shininess);

    Shader lightShader("lightShader.vert", "lightShader.frag");
    Shader simpleDepthShader("simpleDepthShader.vert", "simpleDepthShader.frag");
    //Shader debugShader("debugShader.vert", "debugShader.frag");

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    std::shared_ptr<Texture> depthMap_ptr = std::make_shared<Texture>(
        TextureImporter::newDepthMap(SHADOW_WIDTH, SHADOW_HEIGHT));
    const GLuint depthMapFBO = TextureImporter::bindDepthMapToFBO(depthMap_ptr->id);
    floor.depthMap = depthMap_ptr;
    for (auto& mesh : gameObj.meshes) {
        mesh.depthMap = depthMap_ptr;
    }

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
        
        // Render depth map
        glViewport(0, 0, depthMap_ptr->width, depthMap_ptr->height);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        // Configuration shader
        glm::mat4 lightView = glm::lookAt(dirLight.position, glm::vec3(0.0f, 0.0f, 0.0f),
            glm::vec3(0.0f, 1.0f, 0.0f));
        GLfloat near_plane = 1.0f, far_plane = 7.5f;
        glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;
        simpleDepthShader.use();
        simpleDepthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        glCullFace(GL_FRONT);
        renderScene(simpleDepthShader, floor, gameObj, box);
        glCullFace(GL_BACK);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // reset viewport
        glViewport(0, 0, openglWindow.width, openglWindow.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* 
            Draw Scene
        */
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
        */
        shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
        renderScene(shader, floor, gameObj, box);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}


