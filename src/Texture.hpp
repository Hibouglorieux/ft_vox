/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:41:42 by nathan            #+#    #+#             */
/*   Updated: 2021/12/03 17:54:45 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef TEXTURE_CLASS_H
# define TEXTURE_CLASS_H
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <filesystem>

#include "VoxelGenerator.hpp" //TODO tmp to remove

#define TEXTURECOUNT 8

class Texture {
public:
	Texture(std::string pathToFile = "cat.png");
	Texture(std::vector<std::string> paths, std::string textureName);
	Texture(std::vector<std::string> paths, BigHeightMap& heightMap, float freq = 1.0f, float amp = 1.0f, int octaves = 1, int depth = 0);
	Texture(std::vector<std::string> paths, HeightMap& heightMap, bool smoll);
	Texture(float NoiseTest = 42);
	Texture(bool skybox);
	Texture(HeightMap& heightMap);
	~Texture(void);
	unsigned int getID() const;
private:
	struct TextureCommonData
	{
		unsigned int ID;
		unsigned int nbOfInstance;
		unsigned int blockType;
	};
	static std::map<std::string, TextureCommonData> textureLoaded;
	std::string name;
};

class ResourceManager {
public:
	static void 	loadPack(void);
	static void 	deletePack(void);
	static void 	bindTextures(void);
	static Texture *getBlockTexture(int blockType);
private:
	static std::array<Texture*, TEXTURECOUNT> texturePack;
};
#endif
