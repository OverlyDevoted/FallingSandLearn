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

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int VAO[2], VBO[2], EBO[2];
float mix = 0.5f;
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
    //setting the viewport dimensions. in this case opengl will take the whole viewport if we made it to look smaller the normalized device coordinates would be transformed to fit into 
    //specified witdh and heigth
    glViewport(0, 0, WIDTH, HEIGTH);
    //registering resize callback 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    createTriangle(VAO[0], VBO[0], EBO[0], vertices);
    createTriangle(VAO[1], VBO[1], EBO[1], vertices);


    //shader program object setup
    Shader shader = Shader("src/res/shaders/vertexShader.glsl", "src/res/shaders/fragmentShader.glsl");

    shader.use();
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    glm::mat4 view = glm::mat4(1.0f);
    // note that we're translating the scene in the reverse direction of where we want to move
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

    //render loop, keeps the program open until we tell it to close
    while (!glfwWindowShouldClose(window))
    {
        //dev checks

        //std::cout << glfwGetTime() << std::endl; // gets time


        //check inputs
        processInput(window);

        //draw
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //set uniform for the first shader
        //get uniform location in the shader program getting the location does not require for the program to be used 

        texture1.use(GL_TEXTURE0);
        texture2.use(GL_TEXTURE1);
        //first triangle
        shader.use();
        //set the value based on location. To set the value the program has to be in use hence glUseProgram is called first
        glm::mat4 trans = glm::mat4(1.0f);

        trans = glm::translate(trans, glm::vec3(0.5, -0.5, 0.0));
        trans = glm::rotate(trans, (float)glfwGetTime(), glm::vec3(0.0, 0.0, 1.0));

        shader.setUniform4m("trans", trans);
        shader.setUniform1f("mixF", mix);

        glBindVertexArray(VAO[0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        
        glm::mat4 secondTrans = glm::mat4(1.0f);
        secondTrans = glm::translate(secondTrans, glm::vec3(-0.5, 0.5, 0.0));
        float scaleAmount = (sinf(glfwGetTime()) + 1) / 2;
        secondTrans = glm::scale(secondTrans, glm::vec3(scaleAmount, scaleAmount, scaleAmount));

        std::cout << "Time: " << glfwGetTime() << " SIN: " << sinf(glfwGetTime()) << " Clamped: " << (sinf(glfwGetTime()) + 1) / 2 << std::endl;
        shader.setUniform4m("trans", secondTrans);

        glBindVertexArray(VAO[1]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        //second triangle
        //glUseProgram(shaderProgram2);
        //glBindVertexArray(VAO[1]);
        //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

        //front buffer contains current screen image, while all other draw functions are performed onto the second buffer which is then swapped with this command after drawing all the geometries
        //this prevents user from seeing artifacts from drawn geometries at runtime
        glfwSwapBuffers(window);

        //checks if any event have been triggered (i/o inputs)
        glfwPollEvents();
    }
    //good practice is to delete your vao, vbo and shader programs

    glDeleteBuffers(2, VAO);
    glDeleteBuffers(2, VBO);

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
    else if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        if (mix <= 1.0f)
            mix += 0.0025;
        printf("%f", mix);
    }
    else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        if (mix > 0.0f)
            mix -= 0.0025;
        printf("%f", mix);
    }
}

