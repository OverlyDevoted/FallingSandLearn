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
#define _USE_MATH_DEFINES
#include <cmath>

//my headers
#include "Shader.h"
#include "Texture.h"
#include "MeshGeometry.h"
#include "FallingSandHelper.h"
int WIDTH = 800, HEIGTH = 800;
int simulationWidth = WIDTH, simulationHeigth = HEIGTH;

bool firstMouse = true;
bool isLeftMouseHolding = false;
bool isTabHolding;
int sandType = _SAND;

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

FallingSandHelper sand = FallingSandHelper();
unsigned int initial_size = 4;
bool isRandom = false;
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_click_callback(GLFWwindow* window, int button, int action, int mods);
void processInput(GLFWwindow* window);
glm::vec3 calc_mouse();
void printGLinfo();

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
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

#pragma region SimulationSettings
    //shader program object setup
    Shader shader = Shader("src/res/shaders/fallingSandVertex.glsl", "src/res/shaders/fallingSandFrag.glsl");
    Shader unlitShader = Shader("src/res/shaders/unlitVertex.glsl", "src/res/shaders/unlitFragment.glsl");
    MeshGeometry geometry = MeshGeometry();
    
    initial_size = 200;
    isRandom = true;
    sand.InitializeSpace(initial_size, isRandom);

    unsigned int supposedCamPos = sand.GetSpaceSize();
    simulationWidth = supposedCamPos;
    simulationHeigth = supposedCamPos;
    // cameraPos.y -= supposedCamPos.y - 1;
    const unsigned int starting_cell = sand.GetStartingCells();
    cameraPos = glm::vec3((float)initial_size / 2, (float)initial_size, (float)initial_size);
    glm::vec3 lightPos = glm::vec3((float)initial_size / 2, (float)initial_size, (float)initial_size);
    float fps = 0.0f;
    float timer = 0.0f;
    float iterateWait = 10000.0f;
    float iterateStart = 0.0f;
    float iterateEnd = 0.0f;
    
    float renderTime = 0.0f;
    float iterationTime = 0.0f;
    float fpsCount = 0.0;
    size_t iterationCount = 0;
    size_t maxIterations = 0; 


#pragma endregion
#pragma region Compute Shader Setup
    //compute shader setup
    #pragma region Cells buffer
    struct pos
    {
        GLfloat x, y, z; // positions
        GLint state;
    };
    unsigned int size_sq = initial_size * initial_size;
    unsigned int volume = size_sq * initial_size;
    printf("volume: %d\n", volume);
    GLuint posSSbo; // shader storage buffer object for storing the positions
    glGenBuffers(1, &posSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, posSSbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, volume * sizeof(struct pos), NULL, GL_STATIC_DRAW);
    GLint bufMask = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
    struct pos* points = (struct pos*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, volume * sizeof(struct pos), bufMask);
    int counter = 0;
    for (int i = 0; i < initial_size; i++)
    {
        for (int j = 0; j < initial_size; j++)
        {
            for (int k = 0; k < initial_size; k++)
            {
                points[counter].x = j;
                points[counter].y = initial_size - 1 - i;
                points[counter].z = initial_size - 1 - k;
                if (isRandom)
                    points[counter].state = rand() % 2;
                else if (counter == 26 - (initial_size*2) || counter == 25 - (initial_size * 2) || counter == 17 - (initial_size * 2))
                {
                    points[counter].state = 1;
                }
                else
                    points[counter].state = 0; 
                //printf("index %d: x: %d, y: %d, z: %d\n", counter, j, i, k);
                counter++;
            }
        }
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, posSSbo);
    glEnableVertexAttribArray(4);
    glBindBuffer(GL_ARRAY_BUFFER, posSSbo);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 0, 0);
    #pragma endregion 
    #pragma region swapsBuffer
    struct swaps {
        GLint from;
        GLint to;
    };
    GLuint swapSSbo; // shader storage buffer object for storing the positions
    glGenBuffers(1, &swapSSbo);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, swapSSbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, volume * sizeof(swaps), NULL, GL_STATIC_DRAW);
    
    struct swaps* swaps = (struct swaps*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, volume * sizeof(struct swaps), bufMask);
    counter = 0;
    for (int i = 0; i < initial_size; i++)
    {
        for (int j = 0; j < initial_size; j++)
        {
            for (int k = 0; k < initial_size; k++)
            {
                swaps[counter].from = -1;
                swaps[counter].to = -1;
                counter++;
            }
        }
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, swapSSbo);
    glEnableVertexAttribArray(5);
    glBindBuffer(GL_ARRAY_BUFFER, swapSSbo);
    glVertexAttribPointer(5, 2, GL_INT, GL_FALSE, 0, 0);
    #pragma endregion
#pragma endregion 
#pragma region Compute shader programs
    Shader computeIterationShader   = Shader("src/res/shaders/compute/computeSand.glsl");
    Shader computeSwapsShader       = Shader("src/res/shaders/compute/computeSwaps.glsl");
    Shader forComputeRes            = Shader("src/res/shaders/compute/computeVert.glsl", "src/res/shaders/compute/computeFrag.glsl");
#pragma endregion
    //render loop, keeps the program open until we tell it to close
    while (!glfwWindowShouldClose(window))
    {
        bool iterated =false;
        //update time
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        fps = 1.0f / deltaTime;
        fpsCount += fps;
        #pragma region Inputs
        processInput(window);
        mousePos = calc_mouse();
        if (isLeftMouseHolding)
        {
            sand.SetPixel(glm::uvec3((int)mousePos.x, (int)mousePos.y, (int)mousePos.z), sandType);
        }
        #pragma endregion
        //clear color buffers
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        glm::mat4 projection;
        projection = glm::perspective(glm::radians(50.0f), (float)(WIDTH / HEIGTH), 0.1f, 1000.0f);
        //projection = glm::ortho(0.0f, supposedCamPos.x, 0.0f, supposedCamPos.y, 0.1f, 1000.0f);
        
        //shader.use();
        //shader.setUniform4m("projection", projection);
        //shader.setUniform4m("view", view);
        
        //#pragma endregion
        float renderStart = (float)glfwGetTime();
        #pragma region RenderFallingSand
        
        

        forComputeRes.use();
        forComputeRes.setUniform4m("projection", projection);
        forComputeRes.setUniform4m("view", view);
        //glBindVertexArray(posVao);
        glDrawArrays(GL_POINTS, 0, volume);
        
        //geometry.bindCube();
        /*sand.IterateThroughSpace([&shader, &geometry, &lightPos](glm::uvec3 pos, unsigned char value) {
                
                if (value == 1)
                    return;
                glm::vec3 color = glm::vec3();

                if (value == 2)
                    color = glm::vec3(0.49f, 0.54f, 0.77f);
                else 
                    color = glm::vec3(0.98f, 0.8f, 0.35f);

                shader.setUniform3f("color", color);
                shader.setUniform3f("lightPos", lightPos);

                //we can have model matrix be calculated once in exchange for memory
                
                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3((float)pos.x,-(float)pos.y + initial_size, pos.z));
                model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
                shader.setUniform4m("model", model);
                geometry.drawCubeManual();
            }
        );*/
        #pragma endregion
        float renderEnd = (float)glfwGetTime();

        #pragma region MousePosOnXZ
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(mousePos.x, mousePos.y, mousePos.z));
        model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
        shader.setUniform3f("color", glm::vec3(1.0f,0.0f,0.0f));
        shader.setUniform4m("model", model);
        geometry.drawCubeManual();
        #pragma endregion
        #pragma region UnlitShader
        unlitShader.use();
        unlitShader.setUniform4m("projection", projection);
        unlitShader.setUniform4m("view", view);
        #pragma endregion
        #pragma region LightSource
        
        /*
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(lightPos));
        model = glm::scale(model, glm::vec3(1.0f,1.0f, 1.0f));
        unlitShader.setUniform3f("color", glm::vec3(1.0f, 1.0f, 0.0f));
        unlitShader.setUniform4m("model", model);
        geometry.drawCubeManual();
        */
        #pragma endregion
        /*
            #pragma region Plane
        
        geometry.bindRectangle();
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(initial_size / 2 - 0.5f, -0.6f, initial_size / 2 - 0.5f));
        model = glm::scale(model, glm::vec3(initial_size * 0.5f, 1.0f, initial_size * 0.5f));
        model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        unlitShader.setUniform3f("color", glm::vec3(0.2f, 0.2f, 0.2f));
        unlitShader.setUniform4m("model", model);
        geometry.drawRectangle();
        
        #pragma endregion
        */
        
        #pragma region IterateFallingSand
        timer += deltaTime;
        if (timer > iterateWait)
        {
            timer = 0.0f;
            iterateStart = (float)glfwGetTime();
            computeIterationShader.use();
            computeIterationShader.setUniform1uint("size", initial_size);
            computeIterationShader.setUniform1uint("volume", volume);
            glDispatchCompute(volume, 1, 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);

            computeSwapsShader.use();
            glDispatchCompute(volume, 1, 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            
            //sand.IterateSpace();
            iterateEnd = (float)glfwGetTime();
            iterationCount++;
        }
        #pragma endregion 
        
        #pragma region ImGUI/Metrics
        //for controlling simulation settings
        ImGui::Begin("Simulation control");
        //ImGui::SliderFloat("Mov speed", &moveSpeed, 0.0f, 20.0f);
        ImGui::Text("Camera pos: x:%f y:%f z:%f",cameraPos.x, cameraPos.y, cameraPos.z);
        //ImGui::Text("yaw: %f pitch: %f", yaw, pitch);
        ImGui::Text("FPS: %f", fps);
        ImGui::Text("Cells: %d", sand.GetCellCount());
        //ImGui::Text("Starting cell count:");
        //ImGui::Text("%d", starting_cell);
        ImGui::SliderFloat("Iteration wait", &iterateWait, 0.0f, 5.0f);
        ImGui::Text("Next iteration: %f %", iterateWait == 0? 0.0 : (timer / iterateWait) * 100);
        ImGui::SliderInt("Cell type to place: ", &sandType, 2, 3);
        if (ImGui::Button("Iterate", ImVec2(70, 30)))
        {
            iterated = true;
            iterateStart = (float)glfwGetTime();
            //sand.IterateSpace();
            computeIterationShader.use();
            computeIterationShader.setUniform1uint("size", initial_size);
            computeIterationShader.setUniform1uint("size_sq", size_sq);
            computeIterationShader.setUniform1uint("volume", volume);
            glDispatchCompute(volume, 1, 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
            
            computeSwapsShader.use();
            glDispatchCompute(volume, 1, 1);
            glMemoryBarrier(GL_ALL_BARRIER_BITS);

            iterateEnd = (float)glfwGetTime();
            iterationCount++;
            timer = 0.0f;
        }
        ImGui::Text("Iteration count: %d", iterationCount);
        if (iterated)
        {
            iterationTime += iterateEnd - iterateStart;
        }
        ImGui::Text("Iteration time: %f s", iterateEnd - iterateStart);
        
        renderTime += renderEnd - renderStart;
        ImGui::Text("Render time: %f s", renderEnd - renderStart);
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        #pragma endregion
        //front buffer contains current screen image, while all other draw functions are performed onto the second buffer which is then swapped with this command after drawing all the geometries
        //this prevents user from seeing artifacts from drawn geometries at runtime
        glfwSwapBuffers(window);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        if (maxIterations != 0 && maxIterations-1 < iterationCount)
            glfwSetWindowShouldClose(window, true);

        //checks if any event have been triggered (i/o inputs)
        glfwPollEvents();
    }

    printf("\nSIMULATION RESULTS:\n");

    printf("World size: %d\n", initial_size);
    printf("Is random: %d\n", isRandom);
    printf("Cell count at the end %d\n", sand.GetCellCount());
    printf("Iterations: %d \n", iterationCount);
    printf("Average render time: %f ms\n", renderTime / iterationCount);
    printf("Average iteration time: %f ms\n", iterationTime / iterationCount);
    printf("Average fps: %f\n\n", fpsCount / iterationCount);
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    //good practice is to delete your vao, vbo and shader programs

    glfwDestroyWindow(window);
    //terminate glfw session
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