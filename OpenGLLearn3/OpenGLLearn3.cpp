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
#include <cmath>
//my headers
#include "Shader.h"
#include "Texture.h"
#include "MeshGeometry.h"
#include "FallingSandHelper.h"

const int WIDTH = 800, HEIGTH = 800;
int simulationWidth = WIDTH, simulationHeigth = HEIGTH;

bool firstMouse = true;
bool isLeftMouseHolding = false;
float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = 0.0f;
float lastX = WIDTH / 2.0f; 
float lastY = HEIGTH / 2.0f;
float currentX = WIDTH / 2.0f;
float currentY = HEIGTH / 2.0f;
float moveSpeed = 2.5f;
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 cameraPos = glm::vec3(-.5f, -.5f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

glm::vec3 mousePos;

FallingSandHelper sand = FallingSandHelper();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);

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
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    glEnable(GL_DEPTH_TEST);
    //setting the viewport dimensions. in this case opengl will take the whole viewport if we made it to look smaller the normalized device coordinates would be transformed to fit into 
    //specified witdh and heigth
    glViewport(0, 0, WIDTH, HEIGTH);
    //registering callbacks
    //resize
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //cursor position
    glfwSetCursorPosCallback(window, mouse_callback);
    //mouse clicks
    glfwSetMouseButtonCallback(window, mouse_click_callback);

    //shader program object setup
    Shader shader = Shader("src/res/shaders/fallingSandVertex.glsl", "src/res/shaders/fallingSandFrag.glsl");

    MeshGeometry geometry = MeshGeometry();
    sand.InitializeSpace(50, 50);
    
    glm::vec3 supposedCamPos = sand.GetSpaceSize();
    simulationWidth = supposedCamPos.x;
    simulationHeigth = supposedCamPos.y;
    cameraPos.y -= supposedCamPos.y-1;
    const unsigned int starting_cell = sand.GetStartingCells();

#pragma region IMGUI
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
#pragma endregion

    float* fps = new float(0.0f);
    float timer = 0.0f;
    float iterateWait = 5.0f;
    //render loop, keeps the program open until we tell it to close
    while (!glfwWindowShouldClose(window))
    {
        //update time
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        *fps = 1.0f / deltaTime;

        //check inputs
        processInput(window);
        if (isLeftMouseHolding)
        {
            sand.SetPixel(std::floor(mousePos.x), std::floor(mousePos.y));
        }

        //draw
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        //set uniform for the first shader
        //get uniform location in the shader program getting the location does not require for the program to be used 

        
        //first triangle
        shader.use();
        //set the value based on location. To set the value the program has to be in use hence glUseProgram is called first
        
        geometry.bindRectangle();

        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        
        glm::mat4 projection;
        //projection = glm::perspective(glm::radians(50.0f), (float)(WIDTH / HEIGTH), 0.1f, 1000.0f);
        projection = glm::ortho(0.0f, supposedCamPos.x, 0.0f, supposedCamPos.y, 0.1f, 1000.0f);
        
        shader.setUniform4m("projection", projection);
        shader.setUniform4m("view", view);
        
        sand.IterateThroughSpace([&shader, &geometry](int x, int y, unsigned char value) {
                
                if (value == 1)
                    return;
                glm::vec3 color = glm::vec3();

                if (value == 2)
                    color = glm::vec3(1.0f, 1.0f, 0.0f);
                else
                    color = glm::vec3(0.0f, 0.0f, 1.0f);

                shader.setUniform3f("color", color);

                //we can have model matrix be calculated once in exchange for memory
                
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3((float)x,-(float)y, 0.0f));
                model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
                shader.setUniform4m("model", model);
                geometry.drawRectangle();
            }
        );
        timer += deltaTime;
        
        if (timer > iterateWait)
        {
            timer = 0.0f;
            sand.IterateSpace();
        }
        //for controlling simulation settings
        ImGui::Begin("Simulation control");
        ImGui::SliderFloat("Mov speed", &moveSpeed, 0.0f, 20.0f);
        ImGui::Text("Camera pos:");
        ImGui::Text("x:%f y:%f z:%f",cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::Text("FPS:");
        ImGui::Text("%f", *fps);
        ImGui::Text("Starting cell count:");
        ImGui::Text("%d", starting_cell);
        ImGui::SliderFloat("Iteration speed", &iterateWait, 0.0f, 5.0f);
        ImGui::Text("Next iteration:");
        ImGui::Text("%f %", (timer /iterateWait)*100);

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

    glfwDestroyWindow(window);
    //terminate glfw session
    glfwTerminate();

    delete fps;
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
    
    float cameraSpeed = moveSpeed * deltaTime; // adjust accordingly
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
    
    if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS)
    {
        if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
    
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    currentX = xpos;
    currentY = ypos;
    mousePos = glm::vec3((currentX / WIDTH) * simulationWidth, (currentY / HEIGTH) * simulationHeigth, 0.5f);
    if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
        return;
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

void mouse_click_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT)
    {
        if (action == GLFW_PRESS)
        {
            //probably could do this in the mouse_callback and here only handle the boolean
            //flipped because that's how cells are stored in the array
            //std::cout << std::floor(pos.y) << " " << std::floor(pos.x) << std::endl;
            isLeftMouseHolding = true;
        }
        else if (action == GLFW_RELEASE)
        {
            isLeftMouseHolding = false;
        }
    }
    
}