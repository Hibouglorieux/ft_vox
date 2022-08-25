/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextManager.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 23:41:53 by nathan            #+#    #+#             */
/*   Updated: 2022/08/25 16:45:32 by nathan           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef TEXTMANAGER_CLASS_H
# define TEXTMANAGER_CLASS_H
#include <vector>
#include <string>
#include <map>
#include "Shader.hpp"
#include "Vec3.hpp"
#include "Vec2.hpp"
#include "Utilities.h"
#include <chrono>

#define PRINT_TO_SCREEN(x, ...) TextManager::print(LOCATION, x, ##__VA_ARGS__)

struct Character {
	unsigned int TextureID; // ID handle of the glyph texture
	Vec3   Size;      // Size of glyph
	Vec3   Bearing;   // Offset from baseline to left/top of glyph
	unsigned int Advance;   // Horizontal offset to advance to next glyph
};

class TextManager {
public:
	static bool	init();
	static void	print(const char* caller, std::string str, double duration = 1.0f);
	static void tickPrint();// call only after draws have been made
	static void clear();
private:
	static void	renderText(std::string stringToDraw, float x, float y, float scale, Vec3 color);

	static std::map<std::string, std::pair<std::chrono::time_point<std::chrono::system_clock>, std::string>> stringToRender;
	static Shader* shader;
	static std::map<GLchar, Character> CharacterMap;
	static unsigned int VAO;
	static unsigned int VBO;
};

#endif
