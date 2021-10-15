#include "Shader.hpp"
#include <Utilities.h>

//temporary matrix functions
#include <string.h>
#include <math.h>
#include <algorithm>
using namespace std;

std::map<std::string, Shader::shaderCommonData> Shader::createdShaders = {};

Shader::Shader(const char* vertexPath, const char* fragmentPath) : stringID(std::string(vertexPath) + std::string(fragmentPath)) {
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
	if (createdShaders.find(stringID) == createdShaders.end())
		createdShaders.insert(std::pair<std::string, shaderCommonData>(stringID, {0, 1}));
	else
	{
		createdShaders[stringID].nbOfInstance++;
		return;
	}
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // open files
        vShaderFile.open(std::string("shaders/") + vertexPath);
        fShaderFile.open(std::string("shaders/") + fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");
    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");
    // shader Program
    createdShaders[stringID].ID = glCreateProgram();
    glAttachShader(getID(), vertex);
    glAttachShader(getID(), fragment);
    glLinkProgram(getID());
    checkCompileErrors(getID(), "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Shader::~Shader( void ) 
{
	createdShaders[stringID].nbOfInstance--;
	if (createdShaders[stringID].nbOfInstance == 0)
	{
		glDeleteProgram(getID());
		createdShaders.erase(stringID);
	}
}

void Shader::printShaders( void )
{
	std::cout << "There are currently " << createdShaders.size() << " shaders:" << std::endl;
	for (auto it : createdShaders)
	std::cout << "About Shader " << it.first << ": has ID: " << it.second.ID << " with " << it.second.nbOfInstance << " instances." << std::endl;
}

unsigned int Shader::getID() const
{
	return createdShaders[stringID].ID;
}

void Shader::use() const
{
    glUseProgram(getID());
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(getID(), name.c_str()), (int)value); 
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string &name, int value) const
{ 
    glUniform1i(glGetUniformLocation(getID(), name.c_str()), value); 
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string &name, float value) const
{ 
    glUniform1f(glGetUniformLocation(getID(), name.c_str()), value); 
}

// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
void Shader::checkCompileErrors(unsigned int shader, std::string type)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}
