#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "tool/shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#define STB_IMAGE_IMPLEMENTATION
#include "tool/stb_image.h"
#include <tool/mesh.h>
#include <tool/model.h>
using namespace std;

// windows size
int SCREEN_WIDTH = 1920;
int SCREEN_HEIGHT = 1080;

// clear color
ImVec4 clear_color = ImVec4(0.1, 0.1, 0.1, 1.0);

// camera setting
float Speed = 1.5f;
glm::vec3 cameraPos   = glm::vec3(16.0f, 18.0f, 25.0f);
glm::vec3 cameraFront = glm::vec3(-0.5f, -0.5f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

// mouse setting
float lastX = SCREEN_WIDTH / 2.0;
float lastY = SCREEN_HEIGHT / 2.0;
float yaw   = -90.0f;
float pitch =  0.0f;
bool enableMouse = false;
float fov   =  45.0f;
float sensitivity = 0.05f;

// render setting
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// material
float Shininess = 32;
glm::vec3 Diffuse = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 Ambient = Diffuse;
glm::vec3 Specular = glm::vec3(1.0f, 1.0f, 1.0f);

// Light
bool enabledirlight = true;
bool enablepointlight = true;
bool enablespotlight = true;

// dirLight
glm::vec3 dirdirection = glm::vec3(-2.4f, -0.2f, -0.3f);
glm::vec3 dirlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
float dirambientStrength = 0.15;
float dirspecularStrength = 0.6;
float dirdiffuseStrength = 1.0;
bool dirlightColorchange = false;
bool dirlightMove = false;

// pointLight
glm::vec3 pointlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
glm::vec3 pointlightPos = glm::vec3(10.f, 10.0f, 5.0f);
float pointambientStrength = 0.3;
float pointspecularStrength = 0.6;
float pointdiffuseStrength = 1.0;
float pointconstant = 1.0f;
float pointlinear = 0.09f;
float pointquadratic = 0.032f;
bool pointlightColorchange = false;
bool pointlightMove = false;

// spotLight
glm::vec3 spotlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
float spotambientStrength = 0.15;
float spotspecularStrength = 0.6;
float spotdiffuseStrength = 1.0;
float spotinnerAngle = 12.5f;
float spotouterAngle = 17.5f;
bool spotlightColorchange = false;
bool usdtexlight = false;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window, float cameraSpeed);
unsigned int loadTexture(char const *path);
string Shader::dirName;

int main(int argc, char *argv[])
{
    Shader::dirName = argv[1];
    glfwInit();
    const char *glsl_version = "#version 330";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "MyWindow", NULL, NULL);
    if(window == NULL)
    {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        cout << "Failed to initialize GLAD" << endl;
        return -1;
    }

    // 创建imgui上下文
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    // 设置样式
    ImGui::StyleColorsDark();
    // 设置平台和渲染器
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    Shader myShader("./shader/vertex.glsl", "./shader/fragment.glsl");
    Shader lightShader("./shader/vertex.glsl", "./shader/light.glsl");
    float vertices[] = 
    {
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 
        0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f, 
        -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f,
        0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,
        0.5f,  0.5f,  0.5f, 0.5f,  0.5f, -0.5f,
        0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f, 0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f,
        0.5f, -0.5f,  0.5f, 0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f, 0.5f,  0.5f, -0.5f,
        0.5f,  0.5f,  0.5f, 0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f
    };

    unsigned int VBO, lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    Model myModel("./static/model/nanosuit/nanosuit.obj");
    
    myShader.use(); 
    myShader.setInt("spotLight.tex", 2);

    glEnable(GL_DEPTH_TEST);

    while(!glfwWindowShouldClose(window))
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        float cameraSpeed = Speed * deltaTime;

        processInput(window, cameraSpeed);

        string FPS = to_string(ImGui::GetIO().Framerate);
        string ms = to_string(1000.0f / ImGui::GetIO().Framerate);
        string time = to_string(glfwGetTime());
        string newTitle = "MyWindow - " + ms + " ms/frame " + " fps: " + FPS + " render time: " + time + "s";
        glfwSetWindowTitle(window, newTitle.c_str());

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Sence Setting");

        if (ImGui::CollapsingHeader("Window Settings"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            ImGui::SliderInt("Width", &SCREEN_WIDTH, 1, 2560);
            ImGui::SliderInt("Height", &SCREEN_HEIGHT, 1, 1440);
            ImGui::ColorEdit3("Clear Color", (float *)&clear_color);
            ImGui::SliderFloat("Camera Speed", &Speed, 0.01f, 4.0f);
            ImGui::SliderFloat("Sensitivity", &sensitivity, 0.01f, 0.5f);
            ImGui::Checkbox("Enable Mouse", &enableMouse);
        }
        if (ImGui::CollapsingHeader("Light Setting"), ImGuiTreeNodeFlags_DefaultOpen)
        {
            if (ImGui::Button("lightReset"))
            {
                enabledirlight = true;
                enablepointlight = true;
                enablespotlight = true;

                // dirLight
                dirdirection = glm::vec3(-2.4f, -0.2f, -0.3f);
                dirlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
                dirambientStrength = 0.15;
                dirspecularStrength = 0.6;
                dirdiffuseStrength = 1.0;
                dirlightColorchange = false;
                dirlightMove = false;

                // pointLight
                pointlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
                pointlightPos = glm::vec3(-2.4f, -0.2f, -0.3f);
                pointambientStrength = 0.3;
                pointspecularStrength = 0.6;
                pointdiffuseStrength = 1.0;
                pointconstant = 1.0f;
                pointlinear = 0.09f;
                pointquadratic = 0.032f;
                pointlightColorchange = false;
                pointlightMove = false;

                // spotLight
                spotlightColor = glm::vec3(1.0f, 1.0f, 1.0f);
                spotambientStrength = 0.15;
                spotspecularStrength = 0.6;
                spotdiffuseStrength = 1.0;
                spotinnerAngle = 12.5f;
                spotouterAngle = 17.5f;
                spotlightColorchange = false;
                bool usdtexlight = false;
            }
            ImGui::Checkbox("Dirlight", &enabledirlight);
            ImGui::SameLine(); 
            ImGui::Checkbox("Pointlight", &enablepointlight);
            ImGui::SameLine(); 
            ImGui::Checkbox("Spotlight", &enablespotlight);
            if (ImGui::CollapsingHeader("Directional Light"))
            {
                ImGui::Checkbox("Color Change##Dir", &dirlightColorchange);
                ImGui::SameLine(); 
                ImGui::Checkbox("Light Move##Dir", &dirlightMove);
                ImGui::ColorEdit3("Color##Dir", (float *)&dirlightColor);
                ImGui::SliderFloat3("Direction##Dir", (float *)&dirdirection, 0.0f, 10.0f);
                ImGui::SliderFloat("Ambient Strength##Dir", &dirambientStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Diffuse Strength##Dir", &dirdiffuseStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Specular Strength##Dir", &dirspecularStrength, 0.0f, 1.0f);
            }

            if (ImGui::CollapsingHeader("Point Light"))
            {
                ImGui::Checkbox("Color Change##Point", &pointlightColorchange);
                ImGui::SameLine(); 
                ImGui::Checkbox("Light Move##Point", &pointlightMove);
                ImGui::ColorEdit3("Color##Point", (float *)&pointlightColor);
                ImGui::SliderFloat3("Position##Point", (float *)&pointlightPos, 0.0f, 50.0f);
                ImGui::SliderFloat("Ambient Strength##Point", &pointambientStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Diffuse Strength##Point", &pointdiffuseStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Specular Strength##Point", &pointspecularStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Constant##Point", &pointconstant, 0.0f, 1.0f);
                ImGui::SliderFloat("Linear##Point", &pointlinear, 0.0f, 1.0f);
                ImGui::SliderFloat("Quadratic##Point", &pointquadratic, 0.0f, 1.0f);
            }
            
            if (ImGui::CollapsingHeader("Spot Light"))
            {
                ImGui::Checkbox("Usd Texlight", &usdtexlight);
                ImGui::ColorEdit3("Color##Spot", (float *)&spotlightColor);
                ImGui::SliderFloat("Ambient Strength##Spot", &spotambientStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Diffuse Strength##Spot", &spotdiffuseStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Specular Strength##Spot", &spotspecularStrength, 0.0f, 1.0f);
                ImGui::SliderFloat("Outer Angle##Spot", &spotouterAngle, 0.0f, 180.0f);
                ImGui::SliderFloat("Inner Angle##Spot", &spotinnerAngle, 0.0f, 180.0f);
            }
        }
        ImGui::End();

        // cout << pointlightPos.x << endl;
        // cout << pointlightPos.y << endl;
        // cout << pointlightPos.z << endl;

        if (spotinnerAngle > spotouterAngle)
        {
            spotinnerAngle = spotouterAngle;
        }

        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        myShader.use();
        myShader.setVec3("viewPos", cameraPos);
        myShader.setVec3("material.ambient",  Ambient);
        myShader.setVec3("material.diffuse",  Diffuse);
        myShader.setVec3("material.specular", Specular);
        myShader.setFloat("material.shininess", Shininess);

        myShader.setVec3("dirLight.direction",  dirdirection);
        myShader.setVec3("dirLight.lightColor",  dirlightColor);
        myShader.setFloat("dirLight.ambientStrength", dirambientStrength);
        myShader.setFloat("dirLight.specularStrength", dirspecularStrength);
        myShader.setFloat("dirLight.diffuseStrength", dirdiffuseStrength);
        myShader.setBool("dirLight.enable", enabledirlight);

        myShader.setVec3("pointLight.lightPos",  cameraPos);
        myShader.setVec3("pointLight.lightColor",  pointlightColor);
        myShader.setFloat("pointLight.ambientStrength", pointambientStrength);
        myShader.setFloat("pointLight.specularStrength", pointspecularStrength);
        myShader.setFloat("pointLight.diffuseStrength", pointdiffuseStrength);
        myShader.setFloat("pointLight.constant", pointconstant);
        myShader.setFloat("pointLight.linear", pointlinear);
        myShader.setFloat("pointLight.quadratic", pointquadratic);
        myShader.setBool("pointLight.enable", enablepointlight);

        myShader.setVec3("spotLight.lightPos",  cameraPos);
        myShader.setVec3("spotLight.lightDirection", cameraFront);
        myShader.setVec3("spotLight.lightColor",  spotlightColor);
        myShader.setFloat("spotLight.ambientStrength", spotambientStrength);
        myShader.setFloat("spotLight.specularStrength", spotspecularStrength);
        myShader.setFloat("spotLight.diffuseStrength", spotdiffuseStrength);
        myShader.setFloat("spotLight.outerCutOff",   glm::cos(glm::radians(spotouterAngle)));
        myShader.setFloat("spotLight.cutOff",   glm::cos(glm::radians(spotinnerAngle)));
        myShader.setBool("spotLight.enable", enablespotlight);
        myShader.setBool("spotLight.usetex", usdtexlight);

        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        myShader.setMat4("projection", projection);
        myShader.setMat4("view", view);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        myShader.setMat4("model", model);

        myModel.Draw(myShader);

        lightShader.use();
        
        lightShader.setVec3("lightColor",  pointlightColor);
        lightShader.setBool("enable", enablepointlight);
        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        if (dirlightColorchange)
        {
            dirlightColor.x = sin(glfwGetTime() * 2.0f);
            dirlightColor.y = sin(glfwGetTime() * 0.7f);
            dirlightColor.z = sin(glfwGetTime() * 1.3f);
        }

        if (pointlightColorchange)
        {
            pointlightColor.x = sin(glfwGetTime() * 2.0f);
            pointlightColor.y = sin(glfwGetTime() * 0.7f);
            pointlightColor.z = sin(glfwGetTime() * 1.3f);
        }

        if (spotlightColorchange)
        {
            spotlightColor.x = sin(glfwGetTime() * 2.0f);
            spotlightColor.y = sin(glfwGetTime() * 0.7f);
            spotlightColor.z = sin(glfwGetTime() * 1.3f);
        }

        if (dirlightMove)
        {
            dirdirection.x = sin(glfwGetTime());
            dirdirection.y = cos(glfwGetTime());
        }

        if (pointlightMove)
        {
            pointlightPos.x = pointlightPos.x + 0.1 * sin(glfwGetTime());
            pointlightPos.z = pointlightPos.z + 0.1 * cos(glfwGetTime());
        }

        model = glm::mat4(1.0f);
        model = glm::translate(model, pointlightPos);
        model = glm::scale(model, glm::vec3(0.5f));
        lightShader.setMat4("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
    
    glfwTerminate();

    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window, float cameraSpeed)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    
    // 控制摄像机
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraFront;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS or glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        cameraPos += cameraSpeed * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        cameraPos -= cameraSpeed * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {
        enableMouse = true;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        enableMouse = false;
    }

    // 摄像机重置
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    {
        cameraPos   = glm::vec3(16.0f, 18.0f, 25.0f);
        cameraFront = glm::vec3(-0.5f, -0.5f, -1.0f);
        cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (enableMouse == 0)
    {
        lastX = xpos;
        lastY = ypos;
    }
    else
    {
        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; 
        lastX = xpos;
        lastY = ypos;

        xoffset *= sensitivity;
        yoffset *= sensitivity;

        yaw += xoffset;
        pitch += yoffset;

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}