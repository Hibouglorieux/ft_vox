/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:41:40 by nathan            #+#    #+#             */
/*   Updated: 2021/10/15 21:58:45 by nathan           ###   ########.fr       */
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
