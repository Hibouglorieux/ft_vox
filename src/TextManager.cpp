/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   TextManager.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nathan <unkown@noaddress.com>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/02/25 23:41:51 by nathan            #+#    #+#             */
/*   Updated: 2022/09/13 15:37:06 by nallani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "TextManager.hpp"
#define PATH_TO_FONT "font/arial.ttf"
//#define PATH_TO_FONT "/usr/share/fonts/truetype/ubuntu/Ubuntu-BI.ttf"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Matrix.hpp"
#include "appWindow.hpp"
#include "ft_vox.h"

std::map<std::string, std::pair<std::chrono::time_point<std::chrono::system_clock>, std::string>> TextManager::stringToRender = {};
Shader* TextManager::shader = nullptr;
unsigned int TextManager::VAO = 0;
unsigned int TextManager::VBO = 0;
std::map<GLchar, Character> TextManager::CharacterMap = {};

bool	TextManager::init()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "Failed to init FreeType library" << std::endl;
		return (false);
	}
	FT_Face face;
	if (FT_New_Face(ft, PATH_TO_FONT, 0, &face))
	{
		std::cerr << "Failed to load font for FreeType library" << std::endl;
		return (false);
	}
	FT_Set_Pixel_Sizes(face, 0 , 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "Failed to load character: " << (char)c << " in font" << std::endl;
			return false;
		}
		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				face->glyph->bitmap.width,
				face->glyph->bitmap.rows,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				face->glyph->bitmap.buffer
				);
		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// now store character for later use
		Character character = {
			texture,
			Vec3(face->glyph->bitmap.width, face->glyph->bitmap.rows, 0),
			Vec3(face->glyph->bitmap_left, face->glyph->bitmap_top, 0),
			(unsigned int)face->glyph->advance.x
		};
		CharacterMap[c] = character;
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

	shader = new Shader("text.vert", "text.frag");
	shader->use();
	Matrix orthoMatrix = Matrix::createOrthoMatrix(0, appWindow::getWindowWidth(), 0, appWindow::getWindowHeight(), NEAR, FAR);
    glUniformMatrix4fv(glGetUniformLocation(shader->getID(), "projection"), 1, GL_TRUE, orthoMatrix.exportForGL());
	return true;
}

void TextManager::clear()
{
	delete shader;
	shader = nullptr;
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
	for (auto& pair : CharacterMap)
	{
		glDeleteTextures(1, &pair.second.TextureID);
	}
	CharacterMap.clear();
}

void	TextManager::renderText(std::string stringToDraw, float x, float y, float scale, Vec3 color)
{
    // activate corresponding render state	
    shader->use();
    glUniform3f(glGetUniformLocation(shader->getID(), "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
	glDisable(GL_DEPTH_TEST);
   	glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

    // iterate through all characters
    for (char c : stringToDraw) 
    {
        Character ch = CharacterMap[c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },            
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }           
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glEnable(GL_CULL_FACE);
}

void	TextManager::print(const char* caller, std::string str, double duration)
{
	stringToRender[caller] = std::make_pair<std::chrono::time_point<std::chrono::system_clock>, std::string>(
		std::chrono::system_clock::now() + std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::duration<double, std::ratio<1>>(duration)),
		std::string(str));
		//std::cout << "printed" << str << std::endl;
}

void	TextManager::tickPrint()
{
	int i = 0;
	std::vector<std::string> expiredMessages;
	std::chrono::time_point<std::chrono::system_clock> timeAtDraw = std::chrono::system_clock::now();
	for (auto it : stringToRender)
	{
		std::string& str = std::get<1>(it.second);
		renderText(str, 20, appWindow::getWindowHeight() - i * 15 - 100, 0.8f, Vec3(0.0, 0.0, 0.0));
		if (std::get<0>(it.second) >= timeAtDraw)
			expiredMessages.push_back(it.first);
		i++;
	}
	for (auto it : expiredMessages)
		stringToRender.erase(it);
}
