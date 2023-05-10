// OpenGLLearn3.cpp : This file contains the 'main' function. Program execution begins and ends there.

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
#define _USE_MATH_DEFINES
#include <cmath>
#include <sys/stat.h>
#include <fstream>

#include "src/vendor/imgui/imgui.h"
#include "src/vendor/imgui/imgui_impl_glfw.h"
#include "src/vendor/imgui/imgui_impl_opengl3.h"
#include "src/vendor/imgui/implot.h"
#include "src/vendor/imgui/implot_internal.h"

#include "Shader.h"
#include "Texture.h"
#include "SequentialFallingSand.h"
#include "ParallelFallingSand.h"
#include "IFallingSand.h"
#include "SimulationHelper.h"

#pragma region Viewport variables
int WIDTH = 800, HEIGTH = 800;
int simulationWidth = WIDTH, simulationHeigth = HEIGTH;

bool firstMouse = true;
bool isLeftMouseHolding = false;
bool isTabHolding;
bool iteratedOnce = false;
bool generatedOnce = false;
bool doOnce = false;

float yaw = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch = -45.0f;
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

void printGLinfo();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
glm::vec3 calc_mouse();
void export_data(const int& type, const int& size, const bool& random, float* iterationTimes, const int& iterationCount, const std::pair<int, int>& mem);
bool fileExists(const std::string& filename);
#pragma endregion 

#pragma region Falling sand variables

SimulationHelper sand = SimulationHelper();

int initial_size = 10;
bool isRandom = true;
int sandType = (int)SandType::_SEQUENTIAL_SAND;
bool render = false;
int maxIterations = 50;
#pragma endregion

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGTH, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD");
        return -1;
    }
    printGLinfo();

    //for hiding cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
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
    srand(time(0));

    #pragma region IMGUI
    //IMGUI setup
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO();(void)io;
    
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();
    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
    #pragma endregion  
    #pragma region Additional Simulation Variables
 
    simulationWidth = initial_size;
    simulationHeigth = initial_size;
    cameraPos = glm::vec3((float)initial_size / 2, (float)initial_size, (float)initial_size);
    glm::vec3 lightPos = glm::vec3((float)initial_size / 2, (float)initial_size, (float)initial_size);
    float fps = 0.0f;
    float fpsCount = 0.0;
    
    int iterationCount = 0;
    bool iterate = false;

    float* iterationPlot = new float();
    std::pair<int, int> mem;
    #pragma endregion
    while (!glfwWindowShouldClose(window))
    {
        #pragma region Loop time calculations
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fps = 1.0f / deltaTime;
        fpsCount += fps;
        #pragma endregion 

        #pragma region Input listener
        processInput(window);
        mousePos = calc_mouse();
        #pragma endregion
        
        #pragma region Simulation loop
        //clear color buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection = glm::perspective(glm::radians(50.0f), (float)(WIDTH / HEIGTH), 0.1f, 1000.0f);

        if(render)
            sand.RenderSpace(view, projection);
        
        if (iterate && iterationCount < maxIterations)
        {
            if (!iteratedOnce)
                iteratedOnce = true;

            //should return time spend cpu(sequential) or gpu(parallel)
            float time = sand.IterateSpace();

            iterationPlot[iterationCount] = time;

            iterationCount++;
            if (iterationCount == maxIterations)
                iterate = false;
        }
        #pragma endregion 

        #pragma region Simulation stat UI render
        ImGui::Begin("Simulation control");
        
        ImGui::Text("Camera pos: x:%f y:%f z:%f", cameraPos.x, cameraPos.y, cameraPos.z);
        ImGui::Text("yaw: %f pitch: %f", yaw, pitch);
        std::string text = sandType == (int)SandType::_PARALLEL_SAND ? "Parallel" : "Sequential";
        ImGui::SliderInt("Simulation type", &sandType, 0, 1, text.c_str());
        ImGui::InputInt("Space size", &initial_size, 1, 10);
        if (ImGui::Button("Generate", ImVec2(70, 30)))
        {
            generatedOnce = true;
            mem = sand.InitializeSpace((SandType)sandType, initial_size, isRandom);
        }
        if (generatedOnce)
        {
            ImGui::Checkbox("Renderer", &render);
            ImGui::InputInt("Iterations", &maxIterations, 1,10);
            if (ImGui::Button("Iterate", ImVec2(70, 30)))
            {
                if (iterate)
                    continue;

                iterate = true;
                iterationCount = 0;
                iterationPlot = new float[maxIterations];
                for (int i = 0; i < maxIterations; i++)
                {
                    iterationPlot[i] = 0.0f;
                }
            }

            if (iteratedOnce)
            {
                if (iterationCount == maxIterations)
                    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, IM_COL32(0, 255, 0, 255));
                ImGui::ProgressBar(iterationCount != 0 ? ((float)(iterationCount + 1) / (float)maxIterations) : 0.0f);
                if (iterationCount == maxIterations)
                {
                    ImGui::PopStyleColor();
                    if (ImGui::Button("Export data"))
                    {
                        export_data(sandType, initial_size, isRandom, iterationPlot, maxIterations, mem);
                    }
                }
                if (ImPlot::BeginPlot("Falling Sand performance"))
                {
                    ImPlot::PlotLine("Iteration time in ms", iterationPlot, maxIterations);
                    ImPlot::EndPlot();
                }
            }
        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
    #pragma endregion

        glfwSwapBuffers(window);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwPollEvents();
    }

    printf("\nPROGRAM CLOSED\n");
    
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);

    glfwTerminate();

    return 0;
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    WIDTH = width;
    HEIGTH = height;
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
        if (!isTabHolding)
        {
            firstMouse = true;
            if (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED)
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            else
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        isTabHolding = true;
    }
    else if(glfwGetKey(window, GLFW_KEY_TAB) == GLFW_RELEASE)
    {
        isTabHolding = false;
    }
}
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{

    currentX = xpos;
    currentY = ypos;
    
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
//https://github.com/ocornut/imgui/blob/master/docs/FAQ.md#q-how-can-i-tell-whether-to-dispatch-mousekeyboard-to-dear-imgui-or-my-application
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
glm::vec3 calc_mouse()
{
    float tempPitch = pitch;
    if (tempPitch >= 0)
        return glm::vec3(0.0f,0.0f,0.0f);
    float angle = std::fabs(tempPitch) * M_PI / 180.0f;
    float length = cameraPos.y / std::sinf(angle);
    return glm::vec3(cameraPos + (cameraFront * length));
}
void printGLinfo()
{
    const GLubyte* vendor = glGetString(GL_VENDOR);
    printf("\nGL vendor: %s\n", vendor);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    printf("GL vendor: %s\n", renderer);
    const GLubyte* version = glGetString(GL_VERSION);
    printf("GL version (string): %s\n", version);
    
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    
    printf("GL version (integer): %d.%d\n", major, minor);

    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);
    printf("GLSL version (string): %s\n\n", glslVersion);

    int work_grp_cnt[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

    printf("max global (total) work group counts x:%i y:%i z:%i\n",
        work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

    int work_grp_size[3];

    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &work_grp_size[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &work_grp_size[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &work_grp_size[2]);

    printf("max local (in one shader) work group sizes x:%i y:%i z:%i\n\n",
        work_grp_size[0], work_grp_size[1], work_grp_size[2]);
}
bool fileExists(const std::string& filename)
{
    std::string check = filename + ".txt";
    struct stat buf;
    if (stat(check.c_str(), &buf) != -1)
    {
        return true;
        printf("YEs");
    }
    return false;
}
void export_data(const int& type, const int& size, const bool& random, float* iterationTimes, const int& iterationCount, const std::pair<int, int>& mem)
{
    std::string typeName = type == (int)SandType::_PARALLEL_SAND ? "Parallel" : "Sequential";
    std::string sizeName = std::to_string(size);
    std::string randomName = random ? "random" : "";
    std::string filename = typeName + "_" + sizeName + "_" + randomName;
    filename = filename.insert(0, "src/results/" + typeName + "/");
    std::string tempFilename = filename;
    int counter = 0;
    
    //total; average
    while (fileExists(tempFilename))
    {
        counter++;
        tempFilename = filename;
        tempFilename = tempFilename.append("_" + std::to_string(counter));
    }
    tempFilename = tempFilename.append(".txt");
    printf("Writing to %s\n", tempFilename.c_str());
    std::ofstream file (tempFilename);
    
    if (file.is_open())
    {
        printf("Successfully created/opened file\n");
        file << typeName << " " << sizeName << " " << randomName << std::endl << std::endl;
        file << "Memory in kilobytes:" << std::endl;
        file << "State and pos: " << mem.first << std::endl;
        file << "Swaps: " << mem.second << std::endl << std::endl;
        float total = 0;
        file << "Total number of iterations: " << iterationCount << std::endl << "Iteration times in ms" << std::endl;
        for (int i = 0; i < iterationCount; i++)
        {
            total += iterationTimes[i];
            file << i << ", " << iterationTimes[i] << std::endl;
        }
        file << "Total: " << total << std::endl;
        float average = total / iterationCount;
        file << "Average: " << average << std::endl;

        file.close();
    }
    else
    {
        printf("Could not open file\n");
    }
    if (file.bad())
        printf("A");
}