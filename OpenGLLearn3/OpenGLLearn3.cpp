#include "src/vendor/imgui/imgui.h"
#include "src/vendor/imgui/imgui_impl_glfw.h"
#include "src/vendor/imgui/imgui_impl_opengl3.h"
// OpenGLLearn3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <glad/glad.h>
//GLFW is a library, written in C, specifically targeted at OpenGL. GLFW gives us the bare necessities required for rendering goodies to the screen. It allows us to create an OpenGL context, define window parameters, and handle user input, which is plenty enough for our purposes. 
#include <GLFW/glfw3.h>

//GLM(mathematics)
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


//image lib
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string>
#include <fstream>

//my headers
#include "Shader.h"
#include "Texture.h"
#include "MeshGeometry.h"

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;

float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

unsigned int VAO[2], VBO[2], EBO[2];
float* mix = new float(0.5f);

void createTriangle(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, float* vertices)
{
    int indices[] = { //briaunos. there was a note to start from one
        0,1,2,
        0,3,2
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //assigns an ID to a vbo
    glGenBuffers(1, &VBO);
    //we bind the buffer so any buffer calls we make on the target GL_ARRAY_BUFFER will be used to modify the currently bound buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // so as you can see we specify a buffer GL_ARRAY_BUFFER, this tells opengl that we are configuring that buffer and then with this command we send the vertex data to the gpu
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 32, vertices, GL_STATIC_DRAW);

    //element buffer which says how to render triangles
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //linking vertex attributes is basically saying how to vertex data is stored, what goes into a single vertex
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
const int WIDTH = 800, HEIGTH = 600;
int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGTH, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //check glad
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    //for hiding cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    //setting the viewport dimensions. in this case opengl will take the whole viewport if we made it to look smaller the normalized device coordinates would be transformed to fit into 
    //specified witdh and heigth
    glViewport(0, 0, WIDTH, HEIGTH);
    //registering resize callback 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    //triangle vertices
    float vertices[] = {
        // positions         // colors           //texture coords
        -0.5f,  0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         0.5f, -0.5f, 0.0f,  0.0f, 0.0f, 1.0f,   1.0f, 0.0f,
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, 1.0f,   1.0f, 1.0f
    };
    Texture texture1 = Texture("src/res/textures/container.jpg", GL_RGB, false);
    Texture texture2 = Texture("src/res/textures/Okay.png", GL_RGBA, true);
    //vao and vbo for a triangle setup
    //createTriangle(VAO[0], VBO[0], EBO[0], vertices);
    //createCube(VAO[1], VBO[1]);


    //shader program object setup
    Shader shader = Shader("src/res/shaders/fallingSandVertex.glsl", "src/res/shaders/fallingSandFrag.glsl");

    shader.use();
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);

    MeshGeometry geometry = MeshGeometry();
    
    //projection = glm::rotate(projection, glm::radians(15.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 cubePositions[] = {
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

    //IMGUI setup
    // Setup Dear ImGui context

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    ImGui_ImplOpenGL3_Init("#version 330");
    bool show_demo_window = true;
    bool show_another_window = true;
    float position[] = { 0.0f,0.0f,3.0f };
    //render loop, keeps the program open until we tell it to close
    while (!glfwWindowShouldClose(window))
    {
        //update time

        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        //std::cout << glfwGetTime() << std::endl; // gets time


        //check inputs
        processInput(window);

        //draw
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        //set uniform for the first shader
        //get uniform location in the shader program getting the location does not require for the program to be used 

        texture1.use(GL_TEXTURE0);
        texture2.use(GL_TEXTURE1);
        //first triangle
        shader.use();
        //set the value based on location. To set the value the program has to be in use hence glUseProgram is called first
        
        geometry.bindCube();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(50.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(cubePositions[5]));
        
        shader.setUniform4m("projection", projection);
        shader.setUniform4m("view", view);
        shader.setUniform4m("model", model);
        
        float color[] = { 0.902f, 0.831f, 0.0f, 1.0f};
        shader.setUniform4f("color", color);
        
        geometry.drawCube();

        /*//for rendering sand
        for (int i = 0; i < 10; i++)
        {
            
        }*/

        //for controlling simulation settings
        ImGui::Begin("Simulatioin control");
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        //front buffer contains current screen image, while all other draw functions are performed onto the second buffer which is then swapped with this command after drawing all the geometries
        //this prevents user from seeing artifacts from drawn geometries at runtime
        glfwSwapBuffers(window);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        //checks if any event have been triggered (i/o inputs)
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //good practice is to delete your vao, vbo and shader programs
    glDeleteBuffers(2, VAO);
    glDeleteBuffers(2, VBO);


    glfwDestroyWindow(window);
    //terminate glfw session
    glfwTerminate();
    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraUp;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraUp;
    /*
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    */
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (*mix <= 1.0f)
            *mix += 0.0025f;
        printf("%f", *mix);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (*mix > 0.0f)
            *mix -= 0.0025f;
        printf("%f", *mix);
    }

}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

