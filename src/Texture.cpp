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
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(path, {0, 1}));
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
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(path, {0, 1}));
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
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(path, {0, 1}));
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
	path = paths[0];
	if (textureLoaded.find(path) == textureLoaded.end())
		textureLoaded.insert(std::pair<std::string, TextureCommonData>(path, {0, 1}));
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

	textureLoaded.insert(std::pair<std::string, TextureCommonData>("test", {0, 1}));
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
	return textureLoaded[path].ID;
}
