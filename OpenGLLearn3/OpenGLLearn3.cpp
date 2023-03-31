// OpenGLLearn3.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <fstream>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::string readFromFile(std::string fileName);
void compileShader(const std::string& src, unsigned int type);
unsigned int VAO[2], VBO[2], EBO[2];

void createTriangle(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO, float* vertices)
{
    int indices[] = { //briaunos. there was a note to start from one
        0,1,2
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //assigns an ID to a vbo
    glGenBuffers(1, &VBO);
    //we bind the buffer so any buffer calls we make on the target GL_ARRAY_BUFFER will be used to modify the currently bound buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // so as you can see we specify a buffer GL_ARRAY_BUFFER, this tells opengl that we are configuring that buffer and then with this command we send the vertex data to the gpu
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*9, vertices, GL_STATIC_DRAW);

    //element buffer which says how to render triangles
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    //linking vertex attributes is basically saying how to vertex data is stored, what goes into a single vertex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glEnableVertexAttribArray(0);
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
const int WIDTH=800, HEIGTH=600;
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
    //setting the viewport dimensions. in this case opengl will take the whole viewport if we made it to look smaller the normalized device coordinates would be transformed to fit into 
    //specified witdh and heigth
    glViewport(0, 0, WIDTH, HEIGTH);
    //registering resize callback 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //triangle vertices
    float vertices[] = {
        -1.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        -0.5f, 0.0f, 0.0f
    };
    
    //vao and vbo for a triangle setup
    createTriangle(VAO[0], VBO[0], EBO[0], vertices);

    float vertices2 [] = {
         0.5f, 0.0f, 0.0f,
         1.0f, 0.0f, 0.0f,
         1.0f, 1.0f, 0.0f
    };

    createTriangle(VAO[1], VBO[1], EBO[1], vertices2);
    //shader program object setup
    unsigned int vShader;
    std::string source = readFromFile("../OpenGLLearn3/src/res/shaders/vertexShader.glsl");
    const char* src = source.c_str();
    vShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vShader, 1, &src, NULL);
    glCompileShader(vShader);
    int  success;
    char infoLog[512];
    glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fShader;
    source = readFromFile("../OpenGLLearn3/src/res/shaders/fragmentShader.glsl").c_str();
    src = source.c_str();
    fShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader, 1, &src, NULL);
    glCompileShader(fShader);
    glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    unsigned int fShader2;
    source = readFromFile("../OpenGLLearn3/src/res/shaders/fragmentShader2.glsl").c_str();
    src = source.c_str();
    fShader2 = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fShader2, 1, &src, NULL);
    glCompileShader(fShader2);
    glGetShaderiv(fShader2, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    unsigned int shaderProgram2;
    shaderProgram2 = glCreateProgram();
    glAttachShader(shaderProgram2, vShader);
    glAttachShader(shaderProgram2, fShader2);
    glLinkProgram(shaderProgram2);

    glGetProgramiv(shaderProgram2, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vShader);
    glDeleteShader(fShader);
    glDeleteShader(fShader2);
    //render loop, keeps the program open until we tell it to close
    while (!glfwWindowShouldClose(window))
    {
        //check inputs
        processInput(window);
        
        //draw
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
       
        glUseProgram(shaderProgram2);
        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
       
        //front buffer contains current screen image, while all other draw functions are performed onto the second buffer which is then swapped with this command after drawing all the geometries
        //this prevents user from seeing artifacts from drawn geometries at runtime
        glfwSwapBuffers(window);

        //checks if any event have been triggered (i/o inputs)
        glfwPollEvents();
    }
    //good practice is to delete your vao, vbo and shader programs

    glDeleteBuffers(2, VAO);
    glDeleteBuffers(2, VBO);
    glDeleteProgram(shaderProgram);
    glDeleteProgram(shaderProgram2);

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
    else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        printf("W key pressed");
}

std::string readFromFile(std::string fileName)
{
    std::ifstream shaderFile(fileName);

    if (shaderFile.is_open())
    {
        std::cout << "Opened the file " << fileName << std::endl;
        std::string line;
        std::string shader = "";
        while (std::getline(shaderFile, line))
        {
            line.append("\n");
            shader += line;
        }
        return shader;
    }

    std::cout << "Did not open file" << std::endl;

    return "";
}
