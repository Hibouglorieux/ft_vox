#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
  

class Shader
{
public:
    Shader(const char* vertexPath, const char* fragmentPath);
	virtual ~Shader( void );
	unsigned int getID() const;
	void use() const;
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
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
