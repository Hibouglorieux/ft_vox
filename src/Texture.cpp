/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:41:40 by nathan            #+#    #+#             */
/*   Updated: 2022/02/11 16:16:33 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <GL/glew.h>
#include <functional>// tmp

std::map<std::string, Texture::TextureCommonData> Texture::textureLoaded = {};

Texture::Texture(std::string pathToFile)
{
	name = pathToFile;
	if (textureLoaded.find(name) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(name, {0, 1, 0}));
	else
	{
		textureLoaded[name].nbOfInstance++;
		return;
	}
	int				height;
	int				width;
	int				nr_channel;
	unsigned char	*data;

	std::cout << "added new texture: " << name << textureLoaded[name].ID << std::endl;
	stbi_set_flip_vertically_on_load(true);
	if (!(data = stbi_load(std::string("textures/" + name).c_str(), &width, &height, &nr_channel, 0)))
	{
		std::cerr << "Error: couldn't load image correctly with stbi_load" << std::endl;
		exit(-1);
	}
	glGenTextures(1, &textureLoaded[name].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_2D, textureLoaded[name].ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

Texture::Texture(std::vector<std::string> paths, std::string textureName)
{
	name = textureName;
	if (textureLoaded.find(name) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(name, {0, 1, 0}));
	else
	{
		textureLoaded[name].nbOfInstance++;
		return;
	}
	int				height;
	int				width;
	int				nr_channel;
	unsigned char	*data;

	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &textureLoaded[name].ID);// getting the ID from OpenGL
	std::cout << "added new texture: " << name << textureLoaded[name].ID << std::endl;
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureLoaded[name].ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);//CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);//CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);//CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	for (unsigned int i = 0; i < paths.size(); i++)
	{
		if (!(data = stbi_load(std::string("textures/" + paths[i]).c_str(), &width, &height, &nr_channel, 0)))
		{
			std::cerr << "Error: couldn't load image correctly with stbi_load" << "textures/" << paths[i]<< std::endl;
			exit(-1);
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		stbi_image_free(data);
	}
}

Texture::Texture(std::vector<std::string> paths, BigHeightMap& heightMap, float freq, float amp, int octaves, int depth)
{
	name = paths[0];
	if (textureLoaded.find(name) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(name, {0, 1, 0}));
	else
	{
		textureLoaded[name].nbOfInstance++;
		return;
	}
	int				height;
	int				width;
	int				nr_channel;
	unsigned char	*data;

	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &textureLoaded[name].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureLoaded[name].ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	std::cout << "added new texture: " << name << textureLoaded[name].ID << std::endl;
	printf("%f %f %i %i\n", freq, amp, octaves, depth);
	for (unsigned int i = 0; i < paths.size(); i++)
	{
		if (i < 10)
		{
			float* array = new float[1024*1024];//heightMap.size() * heightMap.size()];
			for (unsigned int y = 0; y < 1024/*heightMap.size()*/; y++)
			{
				for (unsigned int x = 0; x < 1024/*heightMap.size()*/; x++)
					array[y * heightMap.size() + x] = VoxelGenerator::Noise3D(x, depth, y, 0.0f, freq, amp, octaves, 0, 2, 0.65);;//heightMap[y][x];
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, heightMap.size(), heightMap.size(), 0, GL_RED, GL_FLOAT, array);
			delete [] array;
		}
		else
		{
			if (!(data = stbi_load(std::string("textures/" + paths[i]).c_str(), &width, &height, &nr_channel, 0)))
			{
				std::cerr << "Error: couldn't load image correctly with stbi_load" << std::endl;
				exit(-1);
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
	}
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

Texture::Texture(float noiseTest)
{
	(void)noiseTest;
	name = "caveMap";
	if (textureLoaded.find(name) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(name, {0, 1, 0}));
	else
	{
		textureLoaded[name].nbOfInstance++;
		return;
	}
	glGenTextures(1, &textureLoaded[name].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureLoaded[name].ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	std::cout << "added new texture: " << name << textureLoaded[name].ID << std::endl;

	/*int caveMapDepth = 16;
#define NOISE_TEXTURE_WIDTH 16
	float frequency = 20;
	float amplitude = 100;
	int octaves = 10;
	int tableId = 1;
	float lacunarity = 1.5;
	float gain = -5;
	std::function<float(float x, float z)> f = [=](float x, float z){
		x = x / (32);
		z = z / (32);
		return VoxelGenerator::Noise3D(x, caveMapDepth, z, 0, frequency, amplitude, octaves, tableId, lacunarity, gain);};
		*/
#define NOISE_TEXTURE_WIDTH 1024
	std::function<float(float x, float z)> f = [=](float x, float z)
	{
		//Scale to whole worley
		x = x * ((float)WORLEY_SIZE / (float)NOISE_TEXTURE_WIDTH);
		z = z * ((float)WORLEY_SIZE / (float)NOISE_TEXTURE_WIDTH);

		//view from 1 chunk
		x = x / ((float)WIDTH / (float)BLOC_WIDTH_PER_CHUNK);
		z = z / ((float)WIDTH / (float)BLOC_WIDTH_PER_CHUNK);
	
		//view from X chunk
		x *= 16;
		z *= 16;

		/*x = x * ( 32.f / 1024.0f);
		z = z * ( 32.f / 1024.f);*/
		float tmp = VoxelGenerator::getWorleyValueAt(x, 0, z);
		//printf("for x:%f. z:%f i get:%f\n", x, z, tmp);
		return tmp;
	};
	float* array = new float[NOISE_TEXTURE_WIDTH * NOISE_TEXTURE_WIDTH];
	for (unsigned int y = 0; y < NOISE_TEXTURE_WIDTH; y++)
	{
		for (unsigned int x = 0; x < NOISE_TEXTURE_WIDTH; x++)
		{
			array[y * NOISE_TEXTURE_WIDTH + x] = f(x, y);
		}
	}
	for (unsigned int i = 0; i < 6; i++)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, NOISE_TEXTURE_WIDTH, NOISE_TEXTURE_WIDTH, 0, GL_RED, GL_FLOAT, array);
	}
	delete [] array;
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

Texture::Texture(bool skybox)
{
	(void)skybox;
	name = "skybox";
	if (textureLoaded.find(name) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(name, {0, 1, 0}));
	else
	{
		textureLoaded[name].nbOfInstance++;
		return;
	}
	glGenTextures(1, &textureLoaded[name].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureLoaded[name].ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	std::cout << "added new texture: " << name << textureLoaded[name].ID << std::endl; 
	unsigned char* array = new unsigned char[512 * 512 * 3 * sizeof(unsigned char)];
	for (unsigned int i = 0; i < 6; i++)
	{
		//float k = 0;
		float c = 0;
		for (unsigned int j = 0; j < 512 * 512; j++)
		{
			if (j % 512 == 0 && j != 0)
				c += 1;
			if (c >= 255)
				c = 255;
			array[j * 3] = (unsigned char)(c);
			array[j * 3 + 1] = (unsigned char)(c);
			array[j * 3 + 2] = (unsigned char)(c);
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, array);
	}
	delete [] array;
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

Texture::Texture(std::vector<std::string> paths, HeightMap& heightMap, bool smoll)
{
	(void)smoll;
	name = paths[0];
	if (textureLoaded.find(name) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(name, {0, 1, 0}));
	else
	{
		textureLoaded[name].nbOfInstance++;
		return;
	}
	int				height;
	int				width;
	int				nr_channel;
	unsigned char	*data;

	std::cout << "added new texture: " << name << textureLoaded[name].ID << std::endl;
	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &textureLoaded[name].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureLoaded[name].ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	for (unsigned int i = 0; i < paths.size(); i++)
	{
		if (i < 10)
		{
			float* array = new float[heightMap.size() * heightMap.size()];
			for (unsigned int y = 0; y < heightMap.size(); y++)
			{
				for (unsigned int x = 0; x < heightMap.size(); x++)
					array[y * heightMap.size() + x] = heightMap[y][x];
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, heightMap.size(), heightMap.size(), 0, GL_RED, GL_FLOAT, array);
			delete [] array;
		}
		else
		{
			if (!(data = stbi_load(std::string("textures/" + paths[i]).c_str(), &width, &height, &nr_channel, 0)))
			{
				std::cerr << "Error: couldn't load image correctly with stbi_load" << std::endl;
				exit(-1);
			}
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
	}
	//glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
}

Texture::Texture(HeightMap& heightMap)
{
	int height = heightMap.size();
	int width = heightMap[0].size();
	float* array = new float[height * width];
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
			array[y * width + x] = heightMap[y][x];
	}

	textureLoaded.insert(std::pair<std::string, TextureCommonData>("test", {0, 1, 0}));
	name = "test";
	std::cout << "added new texture: " << name << textureLoaded[name].ID << std::endl;
	glGenTextures(1, &textureLoaded[name].ID);
	glBindTexture(GL_TEXTURE_2D, textureLoaded[name].ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, array);
	delete [] array;
}

Texture::~Texture()
{
	textureLoaded[name].nbOfInstance--;
	if (textureLoaded[name].nbOfInstance == 0)
	{
		glDeleteTextures(1, &textureLoaded[name].ID);
		textureLoaded.erase(name);
	}
}

unsigned int	Texture::getID() const
{
	// TODO : Rewrite texture to correctly get texture id ?
	//std::cout << name << std::endl;
	return textureLoaded[name].ID;
}

//	---------------------------------------------------------------------------------------------------------------------------------------------
//
//	RESOURCES PACK MANAGER
//
//  ---------------------------------------------------------------------------------------------------------------------------------------------

#include <dirent.h>
#include <algorithm>
#include <fstream>

std::array<Texture*, TEXTURECOUNT> ResourceManager::texturePack;

void ResourceManager::loadPack(void)
{
	texturePack[BLOCK_WATER] = new Texture({
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"}}, "BLOCK_WATER");

	texturePack[BLOCK_SAND] = new Texture({
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"}}, "BLOCK_SAND");
		
	texturePack[BLOCK_DIRT] = new Texture({
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"}}, "BLOCK_DIRT");
	
	texturePack[BLOCK_GRASS] = new Texture({
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_TOP.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"}}, "BLOCK_GRASS");

	texturePack[BLOCK_GRASS_SNOW] = new Texture({
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"}}, "BLOCK_GRASS_SNOW");

	texturePack[BLOCK_STONE] = new Texture({
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"}}, "BLOCK_STONE");

	texturePack[BLOCK_SNOW] = new Texture({
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"}}, "BLOCK_SNOW");

	texturePack[BLOCK_BEDROCK] = new Texture({
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"}}, "BLOCK_BEDROCK");

}

void ResourceManager::bindTextures(void)
{
	glActiveTexture(GL_TEXTURE0 + BLOCK_WATER); // Texture unit 0
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_WATER]->getID());
	glActiveTexture(GL_TEXTURE0 + BLOCK_SAND); // Texture unit 1
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_SAND]->getID());
	glActiveTexture(GL_TEXTURE0 + BLOCK_DIRT); // Texture unit 2
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_DIRT]->getID());
	glActiveTexture(GL_TEXTURE0 + BLOCK_GRASS); // Texture unit 3
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_GRASS]->getID());
	glActiveTexture(GL_TEXTURE0 + BLOCK_GRASS_SNOW); // Texture unit 4
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_GRASS_SNOW]->getID());
	glActiveTexture(GL_TEXTURE0 + BLOCK_STONE); // Texture unit 5
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_STONE]->getID());
	glActiveTexture(GL_TEXTURE0 + BLOCK_SNOW); // Texture unit 6
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_SNOW]->getID());
	glActiveTexture(GL_TEXTURE0 + BLOCK_BEDROCK); // Texture unit 7
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_BEDROCK]->getID());
}

Texture *ResourceManager::getBlockTexture(int blockType)
{
	return texturePack[blockType];
}

void ResourceManager::deletePack()
{
	for (long unsigned int i = 0; i < texturePack.size(); i++)
	{
		delete texturePack[i];
	}
}
/*
void ResourceManager::loadPack(void)
{
	DIR *dir; struct dirent *diread;
    std::vector<char *> files;
    std::vector<char *> BlockNames;
	
    if ((dir = opendir("./textures/packDefault/")) != nullptr) {
        while ((diread = readdir(dir)) != nullptr) {
			if (diread->d_type == DT_REG)
			{
				std::string name = diread->d_name;
				if (name.substr(name.find_last_of(".") + 1) == "block")
				{
					files.clear();
					std::string line;

					// Read from the text file
					std::ifstream MyReadFile("./textures/packDefault/" + name);
					int lineId = 0;
					char *textureName = "";

					while (getline (MyReadFile, line)) {
					// Output the text from the file
						if (lineId == 0)
						{
							textureName = line;
						}
						else
						{
							files.push_back("")
						}
						lineId++;
					}
					std::cout << std::endl;

					// Close the file
					MyReadFile.close(); 
				}
			}
        }
        closedir(dir);
    } else {
        perror ("opendir");
    }
}*/
