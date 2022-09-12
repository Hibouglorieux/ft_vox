#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
  

class Shader
{
public:
    Shader(const char* vertexPath = "voxCube.vert", const char* fragmentPath = "voxCube.frag");
	virtual ~Shader( void );
	unsigned int getID() const;
	void use() const;
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
    void setMat4(const std::string &name, glm::mat4 &value) const;
	static void printShaders( void ); // debug function;

private:
	struct shaderCommonData
	{
		unsigned int ID;
		unsigned int nbOfInstance;
	};
	static std::map<std::string, shaderCommonData> createdShaders;

    void checkCompileErrors(unsigned int shader, std::string type);
	const std::string stringID;
};
  
#endif
