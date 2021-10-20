/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Texture.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/10/15 16:41:42 by nathan            #+#    #+#             */
/*   Updated: 2021/10/20 18:39:50 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef TEXTURE_CLASS_H
# define TEXTURE_CLASS_H
#include <string>
#include <map>
#include <vector>

#include "VoxelGenerator.hpp" //TODO tmp to remove

class Texture {
public:
	Texture(std::string pathToFile = "cat.png");
	Texture(std::vector<std::string> paths);
	Texture(HeightMap& heightMap);
	~Texture(void);
	unsigned int getID() const;
private:
	struct TextureCommonData
	{
		unsigned int ID;
		unsigned int nbOfInstance;
	};
	static std::map<std::string, TextureCommonData> textureLoaded;
	std::string path;
};

#endif
