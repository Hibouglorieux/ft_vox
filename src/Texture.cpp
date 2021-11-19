/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:41:40 by nathan            #+#    #+#             */
/*   Updated: 2021/10/22 11:33:25 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Texture.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <iostream>
#include <GL/glew.h>

std::map<std::string, Texture::TextureCommonData> Texture::textureLoaded = {};

Texture::Texture(std::string pathToFile)
{
	path = pathToFile;
	if (textureLoaded.find(path) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(path, {0, 1, 0}));
	else
	{
		textureLoaded[path].nbOfInstance++;
		return;
	}
	int				height;
	int				width;
	int				nr_channel;
	unsigned char	*data;

	stbi_set_flip_vertically_on_load(true);
	if (!(data = stbi_load(std::string("textures/" + path).c_str(), &width, &height, &nr_channel, 0)))
	{
		std::cerr << "Error: couldn't load image correctly with stbi_load" << std::endl;
		exit(-1);
	}
	glGenTextures(1, &textureLoaded[path].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_2D, textureLoaded[path].ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(data);
}

Texture::Texture(std::vector<std::string> paths)
{
	path = paths[0];
	if (textureLoaded.find(path) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(path, {0, 1, 0}));
	else
	{
		textureLoaded[path].nbOfInstance++;
		return;
	}
	int				height;
	int				width;
	int				nr_channel;
	unsigned char	*data;

	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &textureLoaded[path].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureLoaded[path].ID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
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

Texture::Texture(std::vector<std::string> paths, BigHeightMap& heightMap)
{
	path = paths[0];
	if (textureLoaded.find(path) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(path, {0, 1, 0}));
	else
	{
		textureLoaded[path].nbOfInstance++;
		return;
	}
	int				height;
	int				width;
	int				nr_channel;
	unsigned char	*data;

	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &textureLoaded[path].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureLoaded[path].ID);
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

Texture::Texture(std::vector<std::string> paths, HeightMap& heightMap, bool smoll)
{
	(void)smoll;
	path = paths[0];
	if (textureLoaded.find(path) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(path, {0, 1, 0}));
	else
	{
		textureLoaded[path].nbOfInstance++;
		return;
	}
	int				height;
	int				width;
	int				nr_channel;
	unsigned char	*data;

	stbi_set_flip_vertically_on_load(false);
	glGenTextures(1, &textureLoaded[path].ID);// getting the ID from OpenGL
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureLoaded[path].ID);
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
	path = "test";
	glGenTextures(1, &textureLoaded[path].ID);
	glBindTexture(GL_TEXTURE_2D, textureLoaded[path].ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, array);
	delete [] array;
}

Texture::~Texture()
{
	textureLoaded[path].nbOfInstance--;
	if (textureLoaded[path].nbOfInstance == 0)
	{
		glDeleteTextures(1, &textureLoaded[path].ID);
		textureLoaded.erase(path);
	}
}

unsigned int	Texture::getID() const
{
	// TODO : Rewrite texture to correctly get texture id ?
	//std::cout << path << std::endl;
	return textureLoaded[path].ID;
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
bool ResourceManager::textureLoaded = false;

void ResourceManager::loadPack(void)
{
	Texture *grass = new Texture({
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_TOP.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"}});

	Texture *grass_snow = new Texture({
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_SIDE.jpg"},
		{"packDefault/GRASS_SIDE.jpg"}});

	Texture *dirt = new Texture({
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"},
		{"packDefault/GRASS_BOTTOM.jpg"}});
	
	Texture *stone = new Texture({
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"},
		{"packDefault/STONE.jpg"}});

	Texture *bedrock = new Texture({
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"},
		{"packDefault/BEDROCK.jpg"}});

	Texture *sand = new Texture({
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"},
		{"packDefault/SAND.jpg"}});

	Texture *water = new Texture({
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"},
		{"packDefault/WATER.png"}});
		
	Texture *snow = new Texture({
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"},
		{"packDefault/SNOW.jpg"}});

	texturePack[BLOCK_WATER] = water;
	texturePack[BLOCK_SAND] = sand;
	texturePack[BLOCK_DIRT] = dirt;
	texturePack[BLOCK_GRASS] = grass;
	texturePack[BLOCK_GRASS_SNOW] = grass_snow;
	texturePack[BLOCK_STONE] = stone;
	texturePack[BLOCK_SNOW] = snow;
	texturePack[BLOCK_BEDROCK] = bedrock;

	ResourceManager::textureLoaded = true;
}

void ResourceManager::bindTextures(void)
{
	glActiveTexture(GL_TEXTURE0 + 10); // Texture unit 1
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_WATER]->getID());
	glActiveTexture(GL_TEXTURE0 + 11); // Texture unit 2
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_SAND]->getID());
	glActiveTexture(GL_TEXTURE0 + 12); // Texture unit 3
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_DIRT]->getID());
	glActiveTexture(GL_TEXTURE0 + 13); // Texture unit 4
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_GRASS]->getID());
	glActiveTexture(GL_TEXTURE0 + 14); // Texture unit 5
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_GRASS_SNOW]->getID());
	glActiveTexture(GL_TEXTURE0 + 15); // Texture unit 6
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_STONE]->getID());
	glActiveTexture(GL_TEXTURE0 + 16); // Texture unit 7
	glBindTexture(GL_TEXTURE_CUBE_MAP, texturePack[BLOCK_SNOW]->getID());
	glActiveTexture(GL_TEXTURE0 + 17); // Texture unit 8
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