#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    unsigned int vShader = compileShaderFile(vertexPath, GL_VERTEX_SHADER);
    unsigned int fShader = compileShaderFile(fragmentPath, GL_FRAGMENT_SHADER);
    int  success;
    char infoLog[512];
    
    ID = glCreateProgram();
    glAttachShader(ID, vShader);
    glAttachShader(ID, fShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    
    glDeleteShader(vShader);
    glDeleteShader(fShader);
}
Shader::Shader(const char* computePath) {
    unsigned int cShader = compileShaderFile(computePath, GL_COMPUTE_SHADER);
    int  success;
    char infoLog[512];

    ID = glCreateProgram();
    glAttachShader(ID, cShader);
    glLinkProgram(ID);

    glGetProgramiv(ID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(ID, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(cShader);
}
Shader::~Shader()
{
    glDeleteProgram(ID);
}

unsigned int Shader::compileShaderFile(const char* filePath, GLuint type)
{
    std::string shader = readFromFile(filePath);
    const char* shaderChar = shader.c_str();
    unsigned int shaderPtr = glCreateShader(type);
    glShaderSource(shaderPtr, 1, &shaderChar, NULL);
    glCompileShader(shaderPtr);
    int  success;
    char infoLog[512];
    glGetShaderiv(shaderPtr, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderPtr, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << type << "::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    return shaderPtr;
}

std::string Shader::readFromFile(std::string fileName)
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
        return shader.c_str();
    }

    std::cout << "Did not open file" << std::endl;

    return nullptr;
}

Shader::Shader()
{
}

void Shader::use()
{
    //checks
    glUseProgram(ID);
}

void Shader::setUniform4m(std::string locationName, glm::mat4 value)
{
    int location = glGetUniformLocation(ID, locationName.c_str());

    glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setUniform1f(std::string locationName, float value)
{
    int vertexColorLocation = glGetUniformLocation(ID, locationName.c_str());

    glUniform1f(vertexColorLocation, value);
}

void Shader::setUniform4f(std::string locationName, float* values)
{
    //needs checking
    int vertexColorLocation = glGetUniformLocation(ID, locationName.c_str());

    glUniform4f(vertexColorLocation, values[0], values[1], values[2], values[3]);
}
void Shader::setUniform3f(std::string locationName, float* values)
{
    //needs checking
    int vertexColorLocation = glGetUniformLocation(ID, locationName.c_str());

    glUniform3f(vertexColorLocation, values[0], values[1], values[2]);
}
void Shader::setUniform3f(std::string locationName, glm::vec3 values)
{
    //needs checking
    int vertexColorLocation = glGetUniformLocation(ID, locationName.c_str());

    glUniform3f(vertexColorLocation, values.x, values.y, values.z);
}
void Shader::setUniform1uint(std::string locationName, unsigned int value)
{
    int location = glGetUniformLocation(ID, locationName.c_str());
    glUniform1ui(location, value);
}